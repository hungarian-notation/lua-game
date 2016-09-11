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
		luaL_Reg functions[] = {
			{ "get_title",			&window_get_title },
			{ "set_title",			&window_set_title },

			{ "get_width",			&window_get_width },
			{ "get_height",			&window_get_height },
			{ "resize",				&window_resize },

			{ "get_should_close",	&window_get_should_close },
			{ "set_should_close",	&window_set_should_close },
			{ "close",				&window_close },

			{ LUA_METAMETHOD_GC,	&release_api_object },
			{ NULL, NULL}
		};

		luaL_setfuncs(L, functions, NULL);
		lua_pushvalue(L, -1);
		lua_setfield(L, -1, LUA_METAMETHOD_INDEX);
	}

	lua_setmetatable(L, -2);

	return 1;
}

int luagame::api::window_get_width(lua_State * L) {
	return 0;
}

int luagame::api::window_get_height(lua_State * L) {
	return 0;
}

int luagame::api::window_resize(lua_State * L) {
	return 0;
}

int luagame::api::window_get_title(lua_State * L) {
	graphics_context * ctx = to_api_object<graphics_context>(L, 1);
	lua_pop(L, 1);
	lua_pushstring(L, ctx->get_title().c_str());
	return 1;
}

int luagame::api::window_set_title(lua_State * L) {
	graphics_context * ctx = to_api_object<graphics_context>(L, 1);
	std::string title = lua_tostring(L, 2);
	ctx->set_title(title);
	return 0;
}

int luagame::api::window_get_should_close(lua_State * L) {
	graphics_context * ctx = to_api_object<graphics_context>(L, 1);
	lua_pushboolean(L, ctx->get_should_close());
	return 1;
}
int luagame::api::window_set_should_close(lua_State * L) {
	graphics_context * ctx = to_api_object<graphics_context>(L, 1);
	bool should_close = lua_toboolean(L, 2);
	ctx->set_should_close(should_close);
	return 0;
}

int luagame::api::window_close(lua_State * L) {
	graphics_context * ctx = to_api_object<graphics_context>(L, 1);
	ctx->set_should_close(true);
	return 0;
}

