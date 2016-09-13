#include "lgapi.h"

#include "..\graphics\window_context.h"

#include <chrono>

using namespace luagame;
using namespace std;


int luaopen_luagame(lua_State * L) {

	luaL_Reg funcs[] = {

		{ "matrix", &lgapi_create_matrix },
		{ "vector", &lgapi_create_vector },

		{ "create_texture", &lgapi_create_texture },
		{ "create_mesh", &lgapi_create_mesh },

		{ "vertex_tostring", &lgapi_vertex_tostring },

		{ NULL, NULL }
	};

	luaL_newlib(L, funcs);

	luagame_newwindow(L);
	lua_setfield(L, -2, "window");

	lua_pushvalue(L, -1);
	lua_setglobal(L, "luagame");

	return 1;
}

namespace {
	int print_error(lua_State * L);
	int lghook_load(lua_State * L);
	int lghook_update(lua_State * L, float dt);
	int lghook_draw(lua_State * L);
}

int luagame_execute() {
	lua_State * L = luaL_newstate();

	luaL_openlibs(L);

	luaopen_luagame(L);

	lua_getfield(L, -1, "window");

	window_context * window = luagame_towindow(L, -1);

	lua_pop(L, 2);

	if (luaL_loadfile(L, "binding\\init.lua") != LUA_OK)
		return print_error(L);

	if (lua_pcall(L, 0, 0, 0) != LUA_OK)
		return print_error(L);

	if (lghook_load(L) != LUA_OK)
		return print_error(L);

	chrono::high_resolution_clock clock;

	chrono::high_resolution_clock::time_point previous, now;

	previous = now = clock.now();

	while (!window->get_should_close()) {
		now = clock.now();
		float delta = chrono::duration_cast<chrono::duration<float>>((now - previous)).count();
		previous = now;

		window->poll_events();

		if (lghook_update(L, delta) != LUA_OK)
			return print_error(L);

		window->clear();

		if (lghook_draw(L) != LUA_OK)
			return print_error(L);

		window->swap_buffers();
	}

	lua_close(L);

	return 0;
}

namespace {
	int call_hook(lua_State * L, const char * hook, int nargs = 0, int nres = 0) {
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

	int lghook_load(lua_State * L) {
		return call_hook(L, "load");
	}

	int lghook_update(lua_State * L, float dt) {
		lua_pushnumber(L, dt);
		return call_hook(L, "update", 1);
	}

	int lghook_draw(lua_State * L) {
		return call_hook(L, "draw");
	}

	int print_error(lua_State * L) {
		_log("lua error: %s", lua_tostring(L, -1));
		lua_close(L);
		return 1;
	}
}