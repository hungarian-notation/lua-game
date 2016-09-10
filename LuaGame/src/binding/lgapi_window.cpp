#include "lgapi_window.h"
#include "..\graphics\graphics_context.h"

static int gc_window(lua_State * L) {
	_log("lua: gc window");
}

int luagame::api::create_window(lua_State * L) {
	_log("lua: create window");

	luagame::graphics_context * ctx = new luagame::graphics_context(); // -- owned by lua

	create_api_object(L, ctx);

	if (luaL_newmetatable(L, "lg_window")) {
		lua_pushcfunction(L, &release_api_object);
		lua_setfield(L, -2, LUA_METAMETHOD_GC);
	}

	lua_setmetatable(L, -2);

	return 1;
}
