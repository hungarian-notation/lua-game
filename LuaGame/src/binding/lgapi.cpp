#include "lgapi.h"

#include "..\graphics\window_context.h"
#include "..\shared_cache.h"

#include "lgmodule.h"

#include <chrono>

using namespace luagame;
using namespace std;

namespace {
	int print_error(lua_State * L);
	int lghook_load(lua_State * L);
	int lghook_update(lua_State * L, float dt);
	int lghook_draw(lua_State * L);
}

int luagame_execute() {
	lua_State * L = luaL_newstate();

	luaL_openlibs(L);

	lua_pushglobaltable(L);

	lgload_luagame(L);

	lua_getfield(L, -1, "window");

	auto window = luagame_getwindow(L);

	lua_pop(L, 2);

	if (luaL_loadfile(L, "binding\\init.lua") != LUA_OK)
		return print_error(L);

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