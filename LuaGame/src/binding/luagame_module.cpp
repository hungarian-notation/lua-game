#include "luagame_module.h"

#include <string>

using namespace luagame;
using namespace std;

luaL_Reg lg_functions[] = {
	{ LG_QUIT, &lg_quit },
	{ NULL, NULL }
};

luaL_Reg lg_metatable[] = {
	{ LUAMETA_INDEX, &lg_index },
	{ LUAMETA_NEWINDEX, &lg_newindex },
	{ NULL, NULL }
};

void lg_setprop(lua_State * L, string key) {	// stack: value
	if (key == LG_PROP_CLOSE) {
		lg_context(L)->set_should_close(lua_toboolean(L, 1));
	} else if (key == LG_PROP_WINDOW_TITLE) {
		lg_context(L)->set_title(lua_tostring(L, 1));
	}
}

int lg_newindex(lua_State * L) {			// stack: luagame, new_key, value
	int libidx = 1;
	int keyidx = 2;
	int validx = 3;

	if (lua_isstring(L, keyidx)) {
		const string key = lua_tostring(L, keyidx);

		if (key == LG_HOOK_INIT ||
			key == LG_HOOK_UPDATE ||
			key == LG_HOOK_DRAW) {
			lua_rawset(L, 1);
			return 0;
		} else {
			lua_insert(L, 1);	// drop [luagame] and [key] from the stack
			lua_settop(L, 1);
			lg_setprop(L, key);
			return 0;
		}
	} else {
		lua_pop(L, 2);
		return 0;
	}
}

int lg_index(lua_State * L) {				// stack: luagame, key
	if (lua_isstring(L, -1)) {
		const string key = lua_tostring(L, -1);

		if (key == LG_PROP_CLOSE) {
			lua_pushboolean(L, lg_context(L)->get_should_close());
			return 1;
		} else if (key == LG_PROP_WINDOW_WIDTH) {
			lua_pushinteger(L, lg_context(L)->get_size().x);
			return 1;
		} else if (key == LG_PROP_WINDOW_HEIGHT) {
			lua_pushinteger(L, lg_context(L)->get_size().y);
			return 1;
		} else if (key == LG_PROP_WINDOW_TITLE) {
			lua_pushstring(L, lg_context(L)->get_title().c_str());
			return 1;
		} else {
			lua_pushnil(L);
			return 1;
		}

	} else {
		lua_pushnil(L);
		return 1;
	}
}

int luagame::luaopen_luagame(lua_State * L) {
	luaL_newlib(L, lg_functions);
	int libidx = lua_gettop(L);

	luagame_state * state = (luagame_state *)lua_newuserdata(L, sizeof(luagame_state));
	state->quit_invoked = false;
	lua_setfield(L, libidx, LG_STATE_KEY);

	luaL_newlib(L, lg_metatable);
	lua_setmetatable(L, libidx);

	return 1;
}

luagame::luagame_state * luagame::get_luagame_state(lua_State * L) {
	lua_getglobal(L, "luagame");
	lua_getfield(L, -1, LG_STATE_KEY);
	luagame_state * state = (luagame_state *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	return state;
}

int lg_quit(lua_State * L) {
	luagame_state * state = get_luagame_state(L);
	state->quit_invoked = true;
	return 0;
}
