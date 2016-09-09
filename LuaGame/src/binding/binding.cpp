#include "binding.h"
#include <stdexcept>

using namespace luagame;

int luagame::binding_execute() {
	lua_State * L = luaL_newstate();

	if (!initialize(L))
		return 1;

	graphics_context * context = new graphics_context();

	luagame_state * state = get_luagame_state(L);

	state->context = context;

	binding_initialize(L);
	
	while (!state->quit_invoked) {
		context->poll_events();
		binding_update(L, 1.0F);
		context->clear();
		binding_draw(L);
		context->swap_buffers();
	}

	lua_close(L);

	context->release();

	return 0;
}

void luagame::print_error(lua_State* L, const char * context) {
	const char* message = lua_tostring(L, -1);
	_log("%s: %s", context, message);
	lua_pop(L, 1);
}

bool luagame::initialize(lua_State * L) {
	luaL_openlibs(L);
	luaopen_luagame(L);
	lua_setglobal(L, LG_NAMESPACE);

	int result;

	result = luaL_loadfile(L, "binding/init.lua");

	if (result != LUA_OK) {
		print_error(L, LG_HOOK_INIT);
		return false;
	}

	result = lua_pcall(L, 0, LUA_MULTRET, 0);

	if (result != LUA_OK) {
		print_error(L, "error in binding initializer");
		return false;
	}

	return true;
}

void luagame::binding_initialize(lua_State * L) {
	lua_getglobal(L, LG_NAMESPACE);
	lua_getfield(L, -1, LG_HOOK_INIT);

	if (lua_isfunction(L, -1)) {
		if (lua_pcall(L, 0, 0, 0) != LUA_OK) 
			print_error(L, "error while initializing client code");
		lua_pop(L, 1);
	} else {
		lua_pop(L, 2);
	}
}

void luagame::binding_update(lua_State * L, float dT) {
	lua_getglobal(L, LG_NAMESPACE);
	lua_getfield(L, -1, LG_HOOK_UPDATE);

	if (lua_isfunction(L, -1)) {
		lua_pushnumber(L, dT);
		if (lua_pcall(L, 1, 0, 0) != LUA_OK)
			print_error(L, "error while updating");
		lua_pop(L, 1);
	} else {
		lua_pop(L, 2);
	}
}

void luagame::binding_draw(lua_State * L) {
	lua_getglobal(L, LG_NAMESPACE);
	lua_getfield(L, -1, LG_HOOK_DRAW);

	if (lua_isfunction(L, -1)) {
		if (lua_pcall(L, 0, 0, 0) != LUA_OK)
			print_error(L, "error while drawing");
		lua_pop(L, 1);
	} else {
		lua_pop(L, 2);
	}
}