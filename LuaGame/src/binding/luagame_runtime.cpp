#include "lgapi.h"

#include "..\window_context.h"
#include "..\shared_cache.h"

#include "lgapi_aux.gen.h"

#include "lgmodule.h"

#include <chrono>
#include <queue>

using namespace luagame;
using namespace std;

namespace {

	struct key_event {
		int key, scancode, action, mods;
	};

	struct mouse_presence_event {
		int entered;
	};

	struct mouse_button_event {
		int button, action, mods;
	};

	struct mouse_move_event {
		double x, y;
	};

	enum game_event_type {
		key,
		mouse_move, mouse_presence, mouse_button
	};

	struct game_event {

		game_event(key_event e) {
			type = game_event_type::key;
			data.key_event = e;
		}

		game_event(mouse_move_event e) {
			type = game_event_type::mouse_move;
			data.mouse_move_event = e;
		}

		game_event(mouse_presence_event e) {
			type = game_event_type::mouse_presence;
			data.mouse_presence_event = e;
		}

		game_event(mouse_button_event e) {
			type = game_event_type::mouse_button;
			data.mouse_button_event = e;
		}

		game_event_type type;

		union {
			key_event key_event;
			mouse_move_event mouse_move_event;
			mouse_presence_event mouse_presence_event;
			mouse_button_event mouse_button_event;
		} data;
	};

	static queue<game_event> event_queue;

	void enqueue_key_event(GLFWwindow * window, int key, int scancode, int action, int mods);

	void enqueue_mouse_move_event(GLFWwindow * window, double xpos, double ypos);
	void enqueue_mouse_presence_event(GLFWwindow * window, int entered);
	void enqueue_mouse_button_event(GLFWwindow * window, int button, int action, int mods);

	int process_events(lua_State * L);

	// -- game loop hooks

	int lghook_load(lua_State * L);
	int lghook_update(lua_State * L, float dt);
	int lghook_draw(lua_State * L);

	// -- event hooks

	int lghook_keypressed(lua_State * L, key_event e);
	int lghook_keyreleased(lua_State * L, key_event e);

	int lghook_mousemoved(lua_State * L, mouse_move_event e);
	int lghook_mouseexit(lua_State * L, mouse_presence_event e);
	int lghook_mouseenter(lua_State * L, mouse_presence_event e);
	int lghook_mousepressed(lua_State * L, mouse_button_event e);
	int lghook_mousereleased(lua_State * L, mouse_button_event e);

	int print_error(lua_State * L);
}

int luagame_execute(std::string game_path) {
	lua_State * L = luaL_newstate();

	luaL_openlibs(L);

	lua_pushglobaltable(L);

	int clean_idx = lua_gettop(L);

	lua_getfield(L, -1, "package");
	lua_getfield(L, -1, "path");

	std::string existing = lua_tostring(L, -1);

	auto new_path = game_path + "?.lua;" + existing  ;

	lua_pop(L, 1);
	lua_pushstring(L, new_path.c_str());
	lua_setfield(L, -2, "path");
	 
	lua_settop(L, clean_idx);
	 
	lgload_luagame(L);

	lua_getfield(L, -1, "window");

	auto window = luagame_getwindow(L);

	glfwSetKeyCallback(window->as_glfw(), enqueue_key_event);
	glfwSetCursorPosCallback(window->as_glfw(), enqueue_mouse_move_event);
	glfwSetCursorEnterCallback(window->as_glfw(), enqueue_mouse_presence_event);
	glfwSetMouseButtonCallback(window->as_glfw(), enqueue_mouse_button_event);

	lua_pop(L, 2);

	if (luagame_loadauxlib(L) != LUA_OK)
	 	return print_error(L);

	if (lua_pcall(L, 0, 0, 0) != LUA_OK)
		return print_error(L);

	luaL_loadstring(L, "require \"main\"");

	if (lua_pcall(L, 0, 0, 0) != LUA_OK)
		return print_error(L);

	if (lghook_load(L) != LUA_OK)
		return print_error(L);

	chrono::high_resolution_clock::time_point previous, now;

	previous = now = chrono::high_resolution_clock::now();

	lua_gc(L, LUA_GCCOLLECT, 0);
	lua_gc(L, LUA_GCSTOP, 0);

	while (!window->get_should_close()) {

		now = chrono::high_resolution_clock::now();
		float delta = chrono::duration_cast<chrono::duration<float>>((now - previous)).count();
		previous = now;

		window->poll_events();

		if (process_events(L) != LUA_OK)
			return print_error(L);

		if (lghook_update(L, delta) != LUA_OK)
			return print_error(L);

		window->clear();

		if (lghook_draw(L) != LUA_OK)
			return print_error(L);

		window->swap_buffers();

		lua_gc(L, LUA_GCSTEP, 0);

	}

	lua_close(L);

	return 0;
}

int luagame_callhook(lua_State * L, const char * hook, int nargs, int nres) {
	lua_getglobal(L, "luagame");

	if (lua_getfield(L, -1, hook) == LUA_TFUNCTION) {
		lua_insert(L, (-2 - nargs));
		lua_pop(L, 1);
		return lua_pcall(L, nargs, nres, 0);
	} else {
		lua_pop(L, 2 + nargs);
		return LUA_OK;
	}
}

namespace {
	int process(lua_State * L, game_event e) {
		switch (e.type) {

		case game_event_type::key:
			return e.data.key_event.action == GLFW_RELEASE ?
				lghook_keyreleased(L, e.data.key_event) :
				lghook_keypressed(L, e.data.key_event);

		case game_event_type::mouse_move:
			return lghook_mousemoved(L, e.data.mouse_move_event);

		case game_event_type::mouse_presence:
			return e.data.mouse_presence_event.entered == 1 ?
				lghook_mouseenter(L, e.data.mouse_presence_event) :
				lghook_mouseexit(L, e.data.mouse_presence_event);

		case game_event_type::mouse_button:
			return e.data.mouse_button_event.action == GLFW_RELEASE ?
				lghook_mousereleased(L, e.data.mouse_button_event) :
				lghook_mousepressed(L, e.data.mouse_button_event);

		default:
			return luaL_error(L, "bad event type");
		}
	}

	int process_events(lua_State * L) {

		while (!event_queue.empty()) {
			int result = process(L, event_queue.front());
			if (result != LUA_OK) return result;
			else event_queue.pop();
		}

		return LUA_OK;
	}

	void enqueue_key_event(GLFWwindow * window, int key, int scancode, int action, int mods) {
		event_queue.push(key_event{ key, scancode, action, mods });
	}

	void enqueue_mouse_move_event(GLFWwindow * window, double xpos, double ypos) {
		event_queue.push(mouse_move_event{ xpos, ypos });
	}

	void enqueue_mouse_presence_event(GLFWwindow * window, int entered) {
		event_queue.push(mouse_presence_event{ entered });
	}

	void enqueue_mouse_button_event(GLFWwindow * window, int button, int action, int mods) {
		event_queue.push(mouse_button_event{ button, action, mods });
	}

	int lghook_keypressed(lua_State * L, key_event event) {

		lua_pushinteger(L, event.key);
		lua_pushinteger(L, event.scancode);
		lua_pushinteger(L, event.mods);
		lua_pushboolean(L, event.action == GLFW_REPEAT);

		return luagame_callhook(L, "key_pressed", 4);
	}

	int lghook_keyreleased(lua_State * L, key_event event) {

		lua_pushinteger(L, event.key);
		lua_pushinteger(L, event.scancode);
		lua_pushinteger(L, event.mods);

		return luagame_callhook(L, "key_released", 3);

	}

	int lghook_mousemoved(lua_State * L, mouse_move_event e) {

		lua_pushnumber(L, e.x);
		lua_pushnumber(L, e.y);
		
		return luagame_callhook(L, "mouse_moved", 2);

	}

	int lghook_mouseexit(lua_State * L, mouse_presence_event e) {

		return luagame_callhook(L, "mouse_exited", 0);
	}

	int lghook_mouseenter(lua_State * L, mouse_presence_event e) {

		return luagame_callhook(L, "mouse_entered", 0);

	}

	int lghook_mousepressed(lua_State * L, mouse_button_event e) {

		lua_pushnumber(L, e.button);
		lua_pushnumber(L, e.action);
		lua_pushnumber(L, e.mods);

		return luagame_callhook(L, "mouse_pressed");

	}

	int lghook_mousereleased(lua_State * L, mouse_button_event e) {

		lua_pushnumber(L, e.button);
		lua_pushnumber(L, e.action);
		lua_pushnumber(L, e.mods);

		return luagame_callhook(L, "mousereleased");

	}

	int lghook_load(lua_State * L) {
		return luagame_callhook(L, "load");
	}

	int lghook_update(lua_State * L, float dt) {
		lua_pushnumber(L, dt);
		return luagame_callhook(L, "update", 1);
	}

	int lghook_draw(lua_State * L) {
		return luagame_callhook(L, "draw");
	}

	int print_error(lua_State * L) {
		_log("lua error: %s", lua_tostring(L, -1));
		lua_close(L);
		return 1;
	}
}

// t.name = func -- where t is the table at idx
void luagame_setfunc(lua_State * L, int idx, const char * name, lua_CFunction func) {
	lua_pushcfunction(L, func);
	lua_setfield(L, idx, name);
}

// t.name = func -- where t is the table at the top of the stack
void luagame_setfunc(lua_State * L, const char * name, lua_CFunction func) {
	lua_pushcfunction(L, func);
	lua_setfield(L, -2, name);
}