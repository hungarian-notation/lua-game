#include "lgapi_window.h"
#include "..\graphics\graphics_context.h"

using namespace luagame;
using namespace luagame::api;

static int gc_window(lua_State * L) {
	_log("lua: gc window");
}

int luagame::api::create_window(lua_State * L) {
	_log("lua: create window");

	luagame::graphics_context * ctx = new luagame::graphics_context(); // -- owned by lua

	create_api_object(L, ctx);

	if (luaL_newmetatable(L, "lg_window")) {
		luaL_Reg functions[] = {
			{ "get_title",			&window::get_title },
			{ "set_title",			&window::set_title },

			{ "get_width",			&window::get_width },
			{ "get_height",			&window::get_height },
			{ "resize",				&window::resize },

			{ "get_should_close",	&window::get_should_close },
			{ "set_should_close",	&window::set_should_close },
			{ "close",				&window::close },

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

int window::get_width(lua_State * L) {
	graphics_context * ctx = to_api_object<graphics_context>(L, 1);
	lua_pushinteger(L, ctx->get_size().x);
	return 1;
}

int window::get_height(lua_State * L) {
	graphics_context * ctx = to_api_object<graphics_context>(L, 1);
	lua_pushinteger(L, ctx->get_size().y);
	return 1;
}

int window::resize(lua_State * L) {
	graphics_context * ctx = to_api_object<graphics_context>(L, 1);
	int width = (int) luaL_checknumber(L, 2);
	int height = (int) luaL_checknumber(L, 3);

	ctx->resize(width, height);

	return 0;
}

int window::get_title(lua_State * L) {
	graphics_context * ctx = to_api_object<graphics_context>(L, 1);
	lua_pushstring(L, ctx->get_title().c_str());
	return 1;
}

int window::set_title(lua_State * L) {
	graphics_context * ctx = to_api_object<graphics_context>(L, 1);
	std::string title = lua_tostring(L, 2);
	ctx->set_title(title);
	return 0;
}

int window::get_should_close(lua_State * L) {
	graphics_context * ctx = to_api_object<graphics_context>(L, 1);
	lua_pushboolean(L, ctx->get_should_close());
	return 1;
}
int window::set_should_close(lua_State * L) {
	graphics_context * ctx = to_api_object<graphics_context>(L, 1);
	bool should_close = lua_toboolean(L, 2);
	ctx->set_should_close(should_close);
	return 0;
}

int window::close(lua_State * L) {
	graphics_context * ctx = to_api_object<graphics_context>(L, 1);
	ctx->set_should_close(true);
	return 0;
}

