#include "../common.h"

#include "lgmodule.h"
#include "lgapi_keydefs.gen.h"

namespace {
	// a table of named keys
	void	include_keys	(lua_State * L);
	int		get_state		(lua_State * L);
}

void lgload_keyboard(lua_State * L) {
	luaL_Reg functions[] = {
		{NULL, NULL}
	};

	luaL_newlib(L, functions);

	include_keys(L);

	lua_setfield(L, -2, "keyboard");
}

namespace {
	void include_keys(lua_State * L) {
		lua_createtable(L, 0, luagame_keydef_count);

		for (int i = 0; i < luagame_keydef_count; i++) {
			lua_pushinteger(L, luagame_keydefs[i].glfw_key);
			lua_setfield(L, -2, luagame_keydefs[i].name);
		}

		lua_setfield(L, -2, "keys");
	}
}
