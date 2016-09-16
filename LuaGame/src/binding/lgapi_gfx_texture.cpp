#include "lgapi_gfx.h"

using namespace luagame;

int lgapi_create_texture(lua_State * L) {
	const char * filename = luaL_checkstring(L, 1);
	auto tex = std::make_shared<luagame::texture_object>(filename);
	luagame_pushobj<texture_object>(L, tex);
	return 1;
}