#include "lgapi_gfx.h"

#include "..\graphics\window_context.h"

using namespace luagame;

namespace {
	int get_title(lua_State * L);
	int set_title(lua_State * L);

	int get_width(lua_State * L);
	int get_height(lua_State * L);
	int resize(lua_State * L);

	int get_should_close(lua_State * L);
	int set_should_close(lua_State * L);
	int close(lua_State * L);
}

void luagame_pushwindow(lua_State * L, window_context * ctx) {
	luagame_pushref(L, ctx);

	if (luaL_newmetatable(L, LUAGAME_WINDOW)) {
		luaL_Reg functions[] = {
			{ "get_title",			&get_title },
			{ "set_title",			&set_title },

			{ "get_width",			&get_width },
			{ "get_height",			&get_height },
			{ "resize",				&resize },

			{ "get_should_close",	&get_should_close },
			{ "set_should_close",	&set_should_close },
			{ "close",				&close },

			{ LUA_METAMETHOD_GC,	&lgapi_releaseref },
			{ NULL, NULL }
		};

		luaL_setfuncs(L, functions, NULL);
		lua_pushvalue(L, -1);
		lua_setfield(L, -1, LUA_METAMETHOD_INDEX);
	}

	lua_setmetatable(L, -2);
}

int luagame_newwindow(lua_State * L) {
	window_context * ctx = new window_context(); // -- owned by lua
	ctx->acquire();
	luagame_pushwindow(L, ctx);
	return 1;
}

luagame::window_context * luagame_towindow(lua_State * L, int idx) {
	return luagame_unwrapref<window_context>(luaL_checkudata(L, idx, LUAGAME_WINDOW));
}

namespace {
	int get_width(lua_State * L) {
		lua_pushinteger(L, luagame_towindow(L, 1)->get_size().x);
		return 1;
	}

	int get_height(lua_State * L) {
		lua_pushinteger(L, luagame_towindow(L, 1)->get_size().y);
		return 1;
	}

	int resize(lua_State * L) {
		int width = (int)luaL_checknumber(L, 2);
		int height = (int)luaL_checknumber(L, 3);

		luagame_towindow(L, 1)->resize(width, height);

		return 0;
	}

	int get_title(lua_State * L) {
		lua_pushstring(L, luagame_towindow(L, 1)->get_title().c_str());
		return 1;
	}

	int set_title(lua_State * L) {
		std::string title = lua_tostring(L, 2);
		luagame_towindow(L, 1)->set_title(title);
		return 0;
	}

	int get_should_close(lua_State * L) {
		lua_pushboolean(L, luagame_towindow(L, 1)->get_should_close());
		return 1;
	}

	int set_should_close(lua_State * L) {
		bool should_close = lua_toboolean(L, 2) != 0;
		luagame_towindow(L, 1)->set_should_close(should_close);
		return 0;
	}

	int close(lua_State * L) {
		luagame_towindow(L, 1)->set_should_close(true);
		return 0;
	}
}