#include "lgmodule.h"
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

void lgload_window(lua_State * L) {
	luaL_Reg functions[] = {
		{ "get_title",			&get_title },
		{ "set_title",			&set_title },

		{ "get_width",			&get_width },
		{ "get_height",			&get_height },

		{ "resize",				&resize },

		{ "get_should_close",	&get_should_close },
		{ "set_should_close",	&set_should_close },
		{ "close",				&close },
		{ NULL, NULL }
	};

	luaL_newlib(L, functions);
	lua_setfield(L, -2, "window");
}

namespace {
	int get_width(lua_State * L) {
		lua_pushinteger(L, luagame_getwindow(L)->get_size().x);
		return 1;
	}

	int get_height(lua_State * L) {
		lua_pushinteger(L, luagame_getwindow(L)->get_size().y);
		return 1;
	}

	int resize(lua_State * L) {
		int width = (int)luaL_checknumber(L, 1);
		int height = (int)luaL_checknumber(L, 2);
		luagame_getwindow(L)->resize(width, height);
		return 0;
	}

	int get_title(lua_State * L) {
		lua_pushstring(L, luagame_getwindow(L)->get_title().c_str());
		return 1;
	}

	int set_title(lua_State * L) {
		std::string title = lua_tostring(L, 1);
		luagame_getwindow(L)->set_title(title);
		return 0;
	}

	int get_should_close(lua_State * L) {
		lua_pushboolean(L, luagame_getwindow(L)->get_should_close());
		return 1;
	}

	int set_should_close(lua_State * L) {
		bool should_close = lua_toboolean(L, 1) != 0;
		luagame_getwindow(L)->set_should_close(should_close);
		return 0;
	}

	int close(lua_State * L) {
		luagame_getwindow(L)->set_should_close(true);
		return 0;
	}
}