#include "lgapi_gfx.h"

using namespace luagame;

void luagame_pushtexture(lua_State * L, texture * tex) {
	luagame_pushref(L, tex);

	if (luaL_newmetatable(L, LUAGAME_TEXTURE)) {
		luaL_Reg functions[] = {
			{ LUA_METAMETHOD_GC,	&lgapi_releaseref },
			{ NULL, NULL }
		};

		luaL_setfuncs(L, functions, NULL);
		lua_pushvalue(L, -1);
		lua_setfield(L, -1, LUA_METAMETHOD_INDEX);
	}

	lua_setmetatable(L, -2);
}

int lgapi_create_texture(lua_State * L) {
	const char * filename = luaL_checkstring(L, 1);
	texture * tex = new luagame::texture(filename);
	luagame_pushtexture(L, tex);
	return 1;
}

int luagame_istexture(lua_State * L, int idx) {
	return (luaL_testudata(L, idx, LUAGAME_TEXTURE) != NULL);
}

texture * luagame_totexture(lua_State * L, int idx) {
	return luagame_unwrapref<texture>(luaL_checkudata(L, idx, LUAGAME_TEXTURE));
}