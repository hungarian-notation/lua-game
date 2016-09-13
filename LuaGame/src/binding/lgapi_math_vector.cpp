#include "lgapi_math.h"

#include <iostream>
#include <cstdlib>
#include <glm\gtx\io.hpp>


#define LUAGAME_SWIZZLE_COMPONENTS	4
#define LUAGAME_SWIZZLE_FAMILIES	3
#define LUAGAME_NOT_A_SYMBOL -1

#define LG_DEFAULT_VCOMP 0

namespace {
	// lua metamethods

	int meta_len(lua_State * L);
	int meta_get(lua_State * L);
	int meta_set(lua_State * L);
	int meta_tostring(lua_State * L);

	// lua operators

	int meta_add(lua_State * L);
	int meta_sub(lua_State * L);
	int meta_mul(lua_State * L);
	int meta_div(lua_State * L);
	int meta_unm(lua_State * L);

	// cartesian (vec3) operations

	int normalize(lua_State * L);
	int get_normalized(lua_State * L);

	int length_squared(lua_State * L);
	int length(lua_State * L);
	int distance(lua_State * L);
	int crossproduct(lua_State * L);
	int dotproduct(lua_State * L);

	// projective (vec4) operations

	int truncate(lua_State * L);
	int get_truncated(lua_State * L);
	int homogenize(lua_State * L);
	int get_homogenized(lua_State * L);
}

void luagame_pushvector(lua_State * L, glm::vec4 vector) {
	glm::vec4 * udata = (glm::vec4*) lua_newuserdata(L, sizeof(glm::vec4));
	*udata = vector;

	if (luaL_newmetatable(L, LUAGAME_VECTOR)) {
		luaL_Reg functions[] = {

			{ LUA_METAMETHOD_LEN,		&meta_len },
			{ LUA_METAMETHOD_INDEX,		&meta_get },
			{ LUA_METAMETHOD_NEWINDEX,	&meta_set },
			{ LUA_METAMETHOD_TOSTRING,	&meta_tostring },

			{ LUA_METAMETHOD_ADD,		&meta_add },
			{ LUA_METAMETHOD_SUB,		&meta_sub },
			{ LUA_METAMETHOD_MUL,		&meta_mul },
			{ LUA_METAMETHOD_DIV,		&meta_div },
			{ LUA_METAMETHOD_UNM,		&meta_unm },

			{ "normalize",				&normalize },
			{ "get_normalized",			&get_normalized },

			{ "length_squared",			&length_squared },
			{ "length",					&length },
			{ "distance",				&distance },
			{ "cross",					&crossproduct },
			{ "dot",					&dotproduct },

			{ "truncate",				&truncate },
			{ "get_truncated",			&get_truncated },
			{ "homogenize",				&homogenize },
			{ "get_homogenized",		&get_homogenized },

			{ NULL, NULL }
		};

		luaL_setfuncs(L, functions, NULL);
	}

	lua_setmetatable(L, -2);
}

void luagame_pushvector(lua_State * L, glm::vec3 vector, float w) {
	luagame_pushvector(L, glm::vec4(vector, w));
}

void luagame_pushvector(lua_State * L, glm::vec2 vector, float z, float w) {
	luagame_pushvector(L, glm::vec4(vector, z, w));
}

int lgapi_create_vector(lua_State * L) {
	glm::vec4 vector;

	switch (lua_gettop(L)) {
	case 0: // default constructor returns a zero-vector

		vector = glm::vec4(LG_DEFAULT_VCOMP, LG_DEFAULT_VCOMP, LG_DEFAULT_VCOMP, LG_DEFAULT_VCOMP);
		break;

		luagame_pushvector(L, glm::vec4());

	case 1:

		if (lua_isnumber(L, 1)) {

			// lgapi_create_vector( <number> ) returns a new vector with the 0th component set to the supplied value

			vector = glm::vec4(
				(float)lua_tonumber(L, 1),
				LG_DEFAULT_VCOMP,
				LG_DEFAULT_VCOMP,
				LG_DEFAULT_VCOMP
			);

		} else if (luagame_isvector(L, 1) || lua_istable(L, 1)) {

			// lgapi_create_vector( <vector> ) or lgapi_create_vector( <table> ) creates a vector from the components of an 
			// existing vector or the elements of a sequence of numbers

			vector = luagame_tovec4(L, 1);

		} else {

			// reject any other single argument

			return luaL_error(L, "invalid single argument to vector constructor: %s", lua_tolstring(L, 1, NULL));

		}

		break;

	case 2:
	case 3:
	case 4:

		// lgapi_create_vector( <num>, <num>, [<num>, [<num>]] ) takes the components of the vector as number arguments

		vector = glm::vec4(
			(float)luaL_checknumber(L, 1),
			(float)luaL_checknumber(L, 2),
			(float)luaL_optnumber(L, 3, LG_DEFAULT_VCOMP),
			(float)luaL_optnumber(L, 4, LG_DEFAULT_VCOMP)
		);

		break;

	default:

		// reject any other number of arguments

		return luaL_error(L, "invalid number of arguments to vector constructor");
	}

	luagame_pushvector(L, vector);
	return 1;
}

int luagame_isvector(lua_State * L, int idx) {
	return (luaL_testudata(L, idx, LUAGAME_VECTOR) != nullptr);
}

glm::vec4 * luagame_tovector(lua_State * L, int idx) {
	glm::vec4 * udata = (glm::vec4*) luaL_checkudata(L, idx, LUAGAME_VECTOR);
	if (udata)
		return udata;
	else {
		luaL_argerror(L, idx, "expected a vector");
		return nullptr;
	}
}

namespace {
	template <int components, class vtype>
	vtype to_value(lua_State * L, int idx) {
		if (luagame_isvector(L, idx))
			return vtype(*luagame_tovector(L, idx));
		else if (lua_istable(L, idx)) {
			vtype value;

			for (int i = 0; i < components; i++) {
				lua_geti(L, idx, i + 1);

				if (lua_isnil(L, -1))
					value[i] = LG_DEFAULT_VCOMP;
				else if (!lua_isnumber(L, -1))
					luaL_error(L, "expected sequence of numbers; found %s at index [%d]", luaL_typename(L, -1), (i + 1));
				else value[i] = (float)lua_tonumber(L, -1);

				lua_pop(L, 1);
			}

			return value;
		} else {
			luaL_error(L, "expected " LUAGAME_VECTOR " or array-like table of %d numbers; found %s", components, luaL_typename(L, idx));
			return vtype(); // unreachable
		}
	}
}

glm::vec4 luagame_tovec4(lua_State * L, int idx) {
	return to_value<4, glm::vec4>(L, idx);
}

glm::vec3 luagame_tovec3(lua_State * L, int idx) {
	return to_value<3, glm::vec3>(L, idx);
}

glm::vec2 luagame_tovec2(lua_State * L, int idx) {
	return to_value<2, glm::vec2>(L, idx);
}

#include <iostream>
#include <sstream>
#include <glm\gtx\io.hpp>

namespace {
	char component_symbols[LUAGAME_SWIZZLE_COMPONENTS][LUAGAME_SWIZZLE_FAMILIES] = {
		{ 'x', 'r', 's' },
		{ 'y', 'g', 't' },
		{ 'z', 'b', 'u' },
		{ 'w', 'a', 'v' }
	};

	int meta_len(lua_State * L) {
		luagame_isvector(L, 1);
		lua_pushinteger(L, 4);
		return 1;
	}

	int check_swizzle_component(lua_State * L, char symbol) {
		for (int index = 0; index < LUAGAME_SWIZZLE_COMPONENTS; ++index)
			for (int family = 0; family < LUAGAME_SWIZZLE_FAMILIES; ++family)
				if (symbol == component_symbols[index][family])
					return index;
		return luaL_error(L, "could not read swizzle-string; '%c' is not a valid component symbol", symbol);
	}

	int set_swizzle(lua_State * L, glm::vec4 * udata, const char * key) {
		int components = strlen(key);

		if (components == 1 && lua_isnumber(L, 3)) { // -- allow the setting of single components via raw numbers
			int dst_index = check_swizzle_component(L, key[0]);
			(*udata)[dst_index] = (float)luaL_checknumber(L, -1);
			return 0;
		} if (lua_istable(L, 3)) { // -- allow the setting of arbitrarily long swizzle strings via tables
			for (int src_index = 0; src_index < components; src_index++) {
				int dst_index = check_swizzle_component(L, key[src_index]);

				if (lua_geti(L, 3, src_index + 1) != LUA_TNUMBER)
					return luaL_error(L, "input to swizzle-setter does not have enough numbers to satisfy swizzle-string");
				else {
					(*udata)[dst_index] = (float)luaL_checknumber(L, -1);
					lua_pop(L, 1);
				}
			}

			return 0;
		} else {
			return luaL_error(L, "could not set \"%s\" on vector object; value is not a number or a sequence");
		}
	}

	int meta_set(lua_State * L) {
		glm::vec4 * udata = luagame_tovector(L, 1);

		if (lua_isinteger(L, 2)) {
			int index = (int)lua_tointeger(L, 2);
			float value = (float)lua_tonumber(L, 3);

			if (index < 1 || index > 4)
				luaL_error(L, "could not set vector component; index %d out of bounds;", index);
			else
				(*udata)[index - 1] = value;

			return 0;
		} else if (lua_isstring(L, 2)) {
			return set_swizzle(L, udata, lua_tostring(L, 2));
		} else {
			return luaL_error(L, "invalid table key: %s", luaL_tolstring(L, 2, NULL));
		}
	}

	int get_swizzle(lua_State * L, glm::vec4 * udata, const char * key) {
		int components = strlen(key);

		if (components == 1) { // -- return single-component values as raw numbers
			int index = check_swizzle_component(L, key[0]);
			lua_pushnumber(L, (*udata)[index]);
			return 1;
		} else { // -- return multi-component values as sequences of numbers
			lua_createtable(L, components, 0);

			for (int i = 0; i < components; i++) {
				int index = check_swizzle_component(L, key[i]);
				lua_pushnumber(L, (*udata)[index]);
				lua_seti(L, -2, i + 1);
			}

			return 1;
		}
	}

	int meta_get(lua_State * L) {
		glm::vec4 * udata = luagame_tovector(L, 1);

		if (lua_isstring(L, 2)) { // -- if the key is a string

			const char * key = lua_tostring(L, 2);

			if (lua_getmetatable(L, 1)) { // -- get the vector userdata's metatable
				lua_getfield(L, -1, key); // -- query that metatable for the field named by the key
				if (!lua_isnil(L, -1)) { // -- if it exists, return it
					return 1; // -- return the member of the vector's metatable that matches the name
				} else return get_swizzle(L, udata, key);
			} else { // -- vector does not have metatable; this should be impossible
				return luaL_error(L, "luagame bug: unexpected state while indexing vector");
			}

		} else if (lua_isnumber(L, 2)) {
			int index = (int)luaL_checkinteger(L, 2);

			if (index < 1 || index > 4)
				return luaL_error(L, "could not set vector component; index %d out of bounds;", index);
			else {
				lua_pushnumber(L, (*udata)[index - 1]);
				return 1;
			}
		} else {
			lua_pushnil(L);
			return 1;
		}
	}

	int meta_tostring(lua_State * L) {
		glm::vec4 udata = *luagame_tovector(L, 1);
		lua_pushfstring(L, "(%f, %f, %f : %f)", udata.x, udata.y, udata.z, udata.w);
		return 1;
	}

	// -- mathematical operators

	int meta_add(lua_State * L) {
		glm::vec4 left = luagame_tovec4(L, 1);
		glm::vec4 right = luagame_tovec4(L, 2);
		luagame_pushvector(L, left + right);
		return 1;
	}

	int meta_sub(lua_State * L) {
		glm::vec4 left = luagame_tovec4(L, 1);
		glm::vec4 right = luagame_tovec4(L, 2);
		luagame_pushvector(L, left - right);
		return 1;
	}

	int meta_unm(lua_State * L) {
		glm::vec4 vector = luagame_tovec4(L, 1);
		luagame_pushvector(L, -vector);
		return 1;
	}

	int meta_mul(lua_State * L) {
		if (luagame_isvector(L, 1) && lua_isnumber(L, 2)) {
			// -- vector * scalar

			luagame_pushvector(L, luagame_tovec4(L, 1) * (float)lua_tonumber(L, 2));
		} else if (lua_isnumber(L, 1) && luagame_isvector(L, 2)) {
			// -- scalar * vector

			luagame_pushvector(L, (float)lua_tonumber(L, 1) * luagame_tovec4(L, 2));
		} else if ((luagame_isvector(L, 1) || lua_istable(L, 1)) && (luagame_isvector(L, 2) || lua_istable(L, 2))) {
			// -- vector * vector

			luagame_pushvector(L, luagame_tovec4(L, 1) * luagame_tovec4(L, 2));
		} else {
			return luaL_error(L, "invalid operands for vector multiplication");
		}

		return 1;
	}

	int meta_div(lua_State * L) {

		if (luagame_isvector(L, 1) && lua_isnumber(L, 2)) {
			// -- vector / scalar

			luagame_pushvector(L, luagame_tovec4(L, 1) / (float)lua_tonumber(L, 2));
		} else if (lua_isnumber(L, 1) && luagame_isvector(L, 2)) {
			// -- scalar / vector

			luagame_pushvector(L, (float)lua_tonumber(L, 1) / luagame_tovec4(L, 2));
		}

		return 1;
	}

	// -- vector math methods

	int normalize(lua_State * L) {
		glm::vec4 * value = luagame_tovector(L, 1);
		*value = glm::vec4(glm::normalize(glm::vec3(*value)), value->w);
		lua_settop(L, 1);
		return 1;
	}

	int get_normalized(lua_State * L) {
		luagame_pushvector(L, glm::normalize(luagame_tovec3(L, 1)));
		return 1;
	}

	int length_squared(lua_State * L) {
		lua_pushnumber(L, glm::length2(luagame_tovec3(L, 1)));
		return 1;
	}

	int length(lua_State * L) {
		lua_pushnumber(L, glm::length(luagame_tovec3(L, 1)));
		return 1;
	}

	int distance(lua_State * L) {
		lua_pushnumber(L, glm::distance(luagame_tovec3(L, 1), luagame_tovec3(L, 2)));
		return 1;
	}

	int crossproduct(lua_State * L) {
		luagame_pushvector(L, glm::cross(luagame_tovec3(L, 1), luagame_tovec3(L, 2)));
		return 1;
	}

	int dotproduct(lua_State * L) {
		lua_pushnumber(L, glm::dot(luagame_tovec3(L, 1), luagame_tovec3(L, 2)));
		return 1;
	}

	int truncate(lua_State * L) {
		glm::vec4 * vector = luagame_tovector(L, 1);
		float w = (float)luaL_optnumber(L, 2, LG_DEFAULT_VCOMP);
		vector->w = w;
		lua_settop(L, 1);
		return 1;
	}

	int get_truncated(lua_State * L) {
		glm::vec4 value = luagame_tovec4(L, 1);
		float w = (float)luaL_optnumber(L, 2, LG_DEFAULT_VCOMP);
		value.w = w;
		luagame_pushvector(L, value);
		return 1;
	}

	int homogenize(lua_State * L) {
		glm::vec4 * vector = luagame_tovector(L, 1);

		if (vector->w == 0) return luaL_error(L, "undefined for vector with homogenous coordinate of 0");
		else {
			*vector = *vector / vector->w;
			lua_settop(L, 1);
			return 1;
		}
	}

	int get_homogenized(lua_State * L) {
		glm::vec4 value = luagame_tovec4(L, 1);

		if (value.w == 0) {
			lua_pushnil(L); // -- undefined
			return 1;
		} else {
			value = value / value.w;
			luagame_pushvector(L, value);
			return 1;
		}
	}
}
