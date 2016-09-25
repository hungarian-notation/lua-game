#include "lgapi_gfx.h"
#include "../graphics/font_object.h"
#include "lgapi_math.h"

using namespace luagame;

namespace {

int get_size(lua_State * L);
int get_character(lua_State * L);
int get_kerning(lua_State * L);

luaL_Reg font_functions[] = {
	{ "get_size",		get_size },
	{ "get_character",	get_character },
	{ "get_kerning",	get_kerning },
	{ NULL,				NULL }
};

}

int lgapi_create_font(lua_State * L) {
	auto ctx = luagame_getcontext(L);

	std::string font_resource = luaL_checkstring(L, 1);
	FT_UInt font_size = luaL_checkinteger(L, 2);

	auto font = std::make_shared<font_object>(&ctx->freetype, font_resource, font_size);

	luagame_pushobj<font_object>(L, font, font_functions, lgapi_create_font);

	return 1;
}

void luagame_pushmetafont(lua_State * L) {
	luagame_pushobjmetatable<font_object>(L, font_functions, lgapi_create_font);
}

namespace {

int get_size(lua_State * L) {
	auto font = luagame_checkobj<font_object>(L, 1);
	lua_pushinteger(L, font->get_size());
	return 1;
}

static char luaX_tochar(lua_State * L, int idx) {
	std::string str = luaL_checkstring(L, 2);
	if (str.length() != 1)
		 luaL_error(L,  "expected a single character string");
	return str[0];
}

int get_character(lua_State * L) {
	auto font = luagame_checkobj<font_object>(L, 1);

	auto cdata = font->get_character(luaX_tochar(L, 2));
	
	lua_newtable(L);
	
	luagame_pushtexture(L, cdata.texture);
	lua_setfield(L, -2, "texture");

	luagame_pushvector(L, cdata.size);
	lua_setfield(L, -2, "size");

	luagame_pushvector(L, cdata.bearing);
	lua_setfield(L, -2, "bearing");

	lua_pushnumber(L, cdata.advance);
	lua_setfield(L, -2, "advance");

	return 1;
}

int get_kerning(lua_State * L) {
	auto font = luagame_checkobj<font_object>(L, 1);

	FT_UInt left = luaX_tochar(L, 2);
	FT_UInt right = luaX_tochar(L, 3);

	glm::vec2 kerning;

	if (font->get_kerning(left, right, &kerning)) {
		luagame_pushvector(L, kerning);
		return 1;
	} else {
		return 0;
	}
}

}