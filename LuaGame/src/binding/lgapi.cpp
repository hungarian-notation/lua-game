#include "lgapi.h"
#include "..\graphics\graphics_context.h"

using namespace luagame;

int print_error(lua_State * L) {
	_log("lua error: %s", lua_tostring(L, -1));
	lua_close(L);
	return 1;
}

int luagame::api::execute() {
	lua_State * L = luaL_newstate();

	luaL_openlibs(L);

	luaopen_luagame(L);

	lua_getfield(L, -1, "window");

	graphics_context * window = to_api_object<graphics_context>(L, -1);

	lua_pop(L, 2);

	if (luaL_loadfile(L, "binding\\init.lua") != LUA_OK)
		return print_error(L);
	
	if (lua_pcall(L, 0, 0, 0) != LUA_OK)
		return print_error(L);

	if (api::hooks::load(L) != LUA_OK)
		return print_error(L);

	while (!window->get_should_close()) {
		window->poll_events();

		if (api::hooks::update(L, 1.0F) != LUA_OK)
			return print_error(L);
		
		window->clear();

		if (api::hooks::draw(L) != LUA_OK)
			return print_error(L);
	
		window->swap_buffers();
	}

	lua_close(L);
}

int luagame::api::luaopen_luagame(lua_State * L) {

	luaL_Reg funcs[] = {
		{ "matrix", &create_matrix },
		{ "vector", &create_vector },
		{ NULL, NULL }
	};

	luaL_newlib(L, funcs);

	api::create_window(L);
	lua_setfield(L, -2, "window");

	lua_pushvalue(L, -1);
	lua_setglobal(L, "luagame");

	return 1;
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
}

int luagame::api::hooks::load(lua_State * L) {
	return call_hook(L, "load");
}

int luagame::api::hooks::update(lua_State * L, float dt) {
	lua_pushnumber(L, dt);
	return call_hook(L, "update", 1);
}

int luagame::api::hooks::draw(lua_State * L) {
	return call_hook(L, "draw");
}
