#include "../common.h"
#include "lgmodule.h"

#include "../window_context.h"

using namespace luagame;

namespace {
	int	set_mode(lua_State * L);

	int	get_position(lua_State * L);
	int	set_position(lua_State * L);
}

void lgload_mouse(lua_State * L) {
	luaL_Reg functions[] = {
		{ "set_mode", set_mode },

		{ "get_position", get_position },
		{ "set_position", set_position },

		{ NULL, NULL }
	};

	luaL_newlib(L, functions);

	lua_setfield(L, -2, "mouse");
}

namespace {
	int set_mode(lua_State * L) {
		const char * cursor_modes[] = {
			"normal", "hidden", "captured", NULL
		};

		window_context::cursor_mode mode = (window_context::cursor_mode) luaL_checkoption(L, 1, NULL, cursor_modes);
		luagame_getwindow(L)->set_cursor_mode(mode);

		return 0;
	}

	int get_position(lua_State * L) {
		glm::dvec2 mouse_position;

		glfwGetCursorPos(luagame_getwindow(L)->as_glfw(), &mouse_position.x, &mouse_position.y);

		lua_pushnumber(L, mouse_position.x);
		lua_pushnumber(L, mouse_position.y);

		return 2;
	}

	int set_position(lua_State * L) {
		double x = luaL_checknumber(L, 1);
		double y = luaL_checknumber(L, 2);

		glfwSetCursorPos(luagame_getwindow(L)->as_glfw(), x, y);
		
		return 0;
	}
}
