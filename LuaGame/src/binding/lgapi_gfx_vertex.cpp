#include "lgapi_gfx.h"
#include "lgapi_math.h"

#include <sstream>
#include <glm\gtx\io.hpp>

using namespace std;

int luagame_isvertex(lua_State * L, int idx) {
	bool isvertex;

	if (lua_istable(L, idx)) {
		lua_getfield(L, idx, LUAGAME_VERTEX_POSITION);
		isvertex = luagame_isvector(L, -1) || lua_istable(L, -1);
		lua_pop(L, 1);
	} else isvertex = false;

	return isvertex;
}

luagame::vertex luagame_checkvertex(lua_State * L, int idx) {
	if (luagame_isvertex(L, idx)) {
		return luagame_tovertex(L, idx);
	} else {
		luaL_argerror(L, idx, "expected vertex table");
		return luagame::vertex();
	}
}

luagame::vertex luagame_tovertex(lua_State * L, int idx) {
	luagame::vertex vert = {};

	if (lua_istable(L, idx)) {

		lua_getfield(L, idx, LUAGAME_VERTEX_POSITION);
		vert.position = luagame_tovec3(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, idx, LUAGAME_VERTEX_COLOR);
		if (!lua_isnil(L, -1))
			vert.color = luagame_tovec3(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, idx, LUAGAME_VERTEX_NORMAL);
		if (!lua_isnil(L, -1))
			vert.normal = luagame_tovec3(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, idx, LUAGAM_VERTEX_TEXCOORD);
		if (!lua_isnil(L, -1))
			vert.texcoord = luagame_tovec2(L, -1);
		lua_pop(L, 1);

	}

	return vert;
}

void luagame_pushvertex(lua_State * L, const luagame::vertex &vert) {
	bool has_color = vert.color != glm::vec3(0, 0, 0);
	bool has_normal = vert.normal != glm::vec3(0, 0, 0);
	bool has_texcoord = vert.texcoord != glm::vec2(0, 0);

	lua_createtable(L, 0, 1 + has_color + has_normal + has_texcoord);

	luagame_pushvector(L, vert.position);
	lua_setfield(L, -2, LUAGAME_VERTEX_POSITION);

	if (has_color) {
		luagame_pushvector(L, vert.color);
		lua_setfield(L, -2, LUAGAME_VERTEX_COLOR);
	}

	if (has_normal) {
		luagame_pushvector(L, vert.normal);
		lua_setfield(L, -2, LUAGAME_VERTEX_NORMAL);
	}

	if (has_texcoord) {
		luagame_pushvector(L, vert.texcoord);
		lua_setfield(L, -2, LUAGAM_VERTEX_TEXCOORD);
	}
}

int lgapi_vertex_tostring(lua_State * L) {
	luagame::vertex vert = luagame_checkvertex(L, 1);

	stringstream buffer;

	buffer << "vertex [" << endl;
	buffer << "  position: " << vert.position << "," << endl;
	buffer << "     color: " << vert.color << "," << endl;
	buffer << "    normal: " << vert.normal << "," << endl;
	buffer << "  texcoord: " << vert.texcoord << endl;
	buffer << "]";

	lua_pushstring(L, buffer.str().c_str());

	return 1;
}