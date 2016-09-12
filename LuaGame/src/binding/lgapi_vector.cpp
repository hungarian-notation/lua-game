#include "lgapi_vector.h"

#include <iostream>
#include <cstdlib>
#include <glm\gtx\io.hpp>


#define LUAGAME_SWIZZLE_COMPONENTS	4
#define LUAGAME_SWIZZLE_FAMILIES	3
#define LUAGAME_NOT_A_SYMBOL -1

#define DEFAULT_HOMOGENOUS_COMPONENT 0

using namespace luagame;

namespace {
	int vector_len(lua_State * L);
	int vector_get(lua_State * L);
	int vector_set(lua_State * L);
	int vector_tostring(lua_State * L);

	int vector_add(lua_State * L);
	int vector_subtract(lua_State * L);
	int vector_multiply(lua_State * L);
	int vector_divide(lua_State * L);
	int vector_negate(lua_State * L);

	int vector_normalize(lua_State * L);
	int vector_get_normalized(lua_State * L);

	int vector_length2(lua_State * L);
	int vector_length(lua_State * L);
	int vector_distance(lua_State * L);
	int vector_crossproduct(lua_State * L);
	int vector_dotproduct(lua_State * L);

	int vector_truncate(lua_State * L);
	int vector_get_truncated(lua_State * L);
	int vector_homogenize(lua_State * L);
	int vector_get_homogenized(lua_State * L);
}

void luagame::push_vector(lua_State * L, glm::vec4 vector) {
	glm::vec4 * udata = (glm::vec4*) lua_newuserdata(L, sizeof(glm::vec4));
	*udata = vector;

	if (luaL_newmetatable(L, LUAGAME_VECTOR)) {
		luaL_Reg functions[] = {

			{ LUA_METAMETHOD_LEN,		&vector_len },
			{ LUA_METAMETHOD_INDEX,		&vector_get },
			{ LUA_METAMETHOD_NEWINDEX,	&vector_set },
			{ LUA_METAMETHOD_TOSTRING,	&vector_tostring },

			// -- These methods operate on component-wise on 4d vectors.

			{ LUA_METAMETHOD_ADD,		&vector_add },
			{ LUA_METAMETHOD_SUB,		&vector_subtract },
			{ LUA_METAMETHOD_MUL,		&vector_multiply },
			{ LUA_METAMETHOD_DIV,		&vector_divide },
			{ LUA_METAMETHOD_UNM,		&vector_negate },

			// -- These methods operate on 3d cartesian vectors.

			{ "normalize",				&vector_normalize },
			{ "get_normalized",			&vector_get_normalized },

			{ "length_squared",			&vector_length2 },
			{ "length",					&vector_length },
			{ "distance",				&vector_distance },
			{ "cross",					&vector_crossproduct },
			{ "dot",					&vector_dotproduct },

			// -- These methods operate on 4d homogenous vectors

			{ "truncate",				&vector_truncate },
			{ "get_truncated",			&vector_get_truncated },
			{ "homogenize",				&vector_homogenize }, 
			{ "get_homogenized",		&vector_get_homogenized },

			{ NULL, NULL }
		};

		luaL_setfuncs(L, functions, NULL);
	}

	lua_setmetatable(L, -2);
}

void luagame::push_vector(lua_State * L, glm::vec3 vector, float w) {
	push_vector(L, glm::vec4(vector, w));
}

int luagame::api::create_vector(lua_State * L) {
	glm::vec4 vector;

	switch (lua_gettop(L)) {
	case 0:
		vector = glm::vec4(0, 0, 0, DEFAULT_HOMOGENOUS_COMPONENT);
		break;
	case 1:

		if (lua_isnumber(L, 1)) {
			vector = glm::vec4( (float)lua_tonumber(L, 1, 0), 0, 0, DEFAULT_HOMOGENOUS_COMPONENT);
		} else if (is_vector(L, 1)) {
			vector = to_vec4(L, 1);
		} else if (lua_istable(L, 1)) {
			for (int comp = 0; comp < 4; comp++) {
				lua_geti(L, 1, comp + 1);
				if (lua_isnumber(L, -1)) {
					vector[comp] = (float)lua_tonumber(L, -1);
				} else vector[comp] = comp == 3 ? DEFAULT_HOMOGENOUS_COMPONENT : 0;
			}
		} else {
			return luaL_error(L, "invalid single argument to vector constructor: %s", lua_tolstring(L, 1, NULL));
		}

		break;
	case 2:
	case 3:
	case 4:
		vector = glm::vec4(
			(float)luaL_optnumber(L, 1, 0),
			(float)luaL_optnumber(L, 2, 0),
			(float)luaL_optnumber(L, 3, 0),
			(float)luaL_optnumber(L, 4, DEFAULT_HOMOGENOUS_COMPONENT)
		);
		break;
	default:
		return luaL_error(L, "invalid number of arguments to vector constructor");
	}

	push_vector(L, vector);
	return 1;
}

int luagame::is_vector(lua_State * L, int idx) {
	return (luaL_testudata(L, idx, LUAGAME_VECTOR) != nullptr);
}

glm::vec4 * luagame::to_vector(lua_State * L, int idx) {
	glm::vec4 * udata = (glm::vec4*) luaL_checkudata(L, idx, LUAGAME_VECTOR);
	if (udata)
		return udata;
	else luaL_argerror(L, idx, "expected a vector");
}

glm::vec4 luagame::to_vec4(lua_State * L, int idx) {
	return *to_vector(L, idx);
}

glm::vec3 luagame::to_vec3(lua_State * L, int idx) {
	return glm::vec3(luagame::to_vec4(L, idx));
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

	int vector_len(lua_State * L) {
		luagame::is_vector(L, 1);
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

	int vector_set_swizzle(lua_State * L, glm::vec4 * udata, const char * key) {
		int components = strlen(key);

		if (components == 1 && lua_isnumber(L, 3)) { // -- allow the setting of single components via raw numbers
			int dst_index = check_swizzle_component(L, key[0]);
			(*udata)[dst_index] = luaL_checknumber(L, -1);
			return 0;
		} if (lua_istable(L, 3)) { // -- allow the setting of arbitrarily long swizzle strings via tables
			for (int src_index = 0; src_index < components; src_index++) {
				int dst_index = check_swizzle_component(L, key[src_index]);

				if (lua_geti(L, 3, src_index + 1) != LUA_TNUMBER)
					return luaL_error(L, "input to swizzle-setter does not have enough numbers to satisfy swizzle-string");
				else {
					(*udata)[dst_index] = luaL_checknumber(L, -1);
					lua_pop(L, 1);
				}
			}

			return 0;
		} else {
			return luaL_error(L, "could not set \"%s\" on vector object; value is not a number or a sequence");
		}
	}

	int vector_set(lua_State * L) {
		glm::vec4 * udata = luagame::to_vector(L, 1);

		if (lua_isinteger(L, 2)) {
			int index = lua_tointeger(L, 2);
			float value = lua_tonumber(L, 3);

			if (index < 1 || index > 4)
				luaL_error(L, "could not set vector component; index %d out of bounds;", index);
			else
				(*udata)[index - 1] = value;

			return 0;
		} else if (lua_isstring(L, 2)) {
			return vector_set_swizzle(L, udata, lua_tostring(L, 2));
		} else {
			return luaL_error(L, "invalid table key: %s", luaL_tolstring(L, 2, NULL));
		}
	}

	int vector_get_swizzle(lua_State * L, glm::vec4 * udata, const char * key) {
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

	int vector_get(lua_State * L) {
		glm::vec4 * udata = luagame::to_vector(L, 1);

		if (lua_isstring(L, 2)) { // -- if the key is a string

			const char * key = lua_tostring(L, 2);

			if (lua_getmetatable(L, 1)) { // -- get the vector userdata's metatable
				lua_getfield(L, -1, key); // -- query that metatable for the field named by the key
				if (!lua_isnil(L, -1)) { // -- if it exists, return it
					return 1; // -- return the member of the vector's metatable that matches the name
				} else return vector_get_swizzle(L, udata, key);
			} else { // -- vector does not have metatable; this should be impossible
				luaL_error(L, "luagame bug: unexpected state while indexing vector");
			}

		} else if (lua_isnumber(L, 2)) {
			int index = luaL_checkinteger(L, 2);

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

	int vector_tostring(lua_State * L) {
		glm::vec4 udata = *luagame::to_vector(L, 1);
		lua_pushfstring(L, "(%f, %f, %f : %f)", udata.x, udata.y, udata.z, udata.w);
		return 1;
	}

	// -- mathematical operators

	int vector_add(lua_State * L) {
		glm::vec4 left = to_vec4(L, 1);
		glm::vec4 right = to_vec4(L, 2);
		push_vector(L, left + right);
		return 1;
	}

	int vector_subtract(lua_State * L) {
		glm::vec4 left = to_vec4(L, 1);
		glm::vec4 right = to_vec4(L, 2);
		push_vector(L, left - right);
		return 1;
	}

	int vector_negate(lua_State * L) {
		glm::vec4 vector = to_vec4(L, 1);
		push_vector(L, -vector);
		return 1;
	}

	int vector_multiply(lua_State * L) {
		if (is_vector(L, 1) && lua_isnumber(L, 2)) {
			// -- vector * scalar

			push_vector(L, to_vec4(L, 1) * (float)lua_tonumber(L, 2));

		} else if (lua_isnumber(L, 1) && is_vector(L, 2)) {
			// -- scalar * vector

			push_vector(L, (float)lua_tonumber(L, 1) * to_vec4(L, 2));

		} else if (is_vector(L, 1) && is_vector(L, 2)) {
			// -- vector * vector

			push_vector(L, to_vec4(L, 1) * to_vec4(L, 2));

			/*

			} else if (is_vector(L, 1) && is_matrix(L, 2)) {
			// -- vector * matrix

			glm::vec4 vector = to_vec4(L, 1);
			glm::mat4 matrix = to_mat4(L, 2);
			push_vector(L, vector * matrix);

			} else if (is_matrix(L, 1) && is_vector(L, 2)) {
			// -- matrix * vector
			_bug("this should be handled by the matrix");

			*/

		} else {
			return luaL_error(L, "invalid operands for vector multiplication");
		}

		return 1;
	}

	int vector_divide(lua_State * L) {

		if (is_vector(L, 1) && lua_isnumber(L, 2)) {
			// -- vector / scalar

			push_vector(L, to_vec4(L, 1) / (float)lua_tonumber(L, 2));
		} else if (lua_isnumber(L, 1) && is_vector(L, 2)) {
			// -- scalar / vector

			push_vector(L, (float)lua_tonumber(L, 1) / to_vec4(L, 2));
		}

		return 1;
	}

	// -- vector math methods

	int vector_normalize(lua_State * L) {
		glm::vec4 * value = to_vector(L, 1);
		*value = glm::vec4(glm::normalize(glm::vec3(*value)), value->w);
		lua_settop(L, 1);
		return 1;
	}

	int vector_get_normalized(lua_State * L) {
		push_vector(L, glm::normalize(to_vec3(L, 1)));
		return 1;
	}

	int vector_length2(lua_State * L) {
		lua_pushnumber(L, glm::length2(to_vec3(L, 1)));
		return 1;
	}

	int vector_length(lua_State * L) {
		lua_pushnumber(L, glm::length(to_vec3(L, 1)));
		return 1;
	}

	int vector_distance(lua_State * L) {
		lua_pushnumber(L, glm::distance(to_vec3(L, 1), to_vec3(L, 2)));
		return 1;
	}

	int vector_crossproduct(lua_State * L) {
		push_vector(L, glm::cross(to_vec3(L, 1), to_vec3(L, 2)));
		return 1;
	}

	int vector_dotproduct(lua_State * L) {
		lua_pushnumber(L, glm::dot(to_vec3(L, 1), to_vec3(L, 2)));
		return 1;
	}

	int vector_truncate(lua_State * L) {
		glm::vec4 * vector = to_vector(L, 1);
		float w = luaL_optnumber(L, 2, DEFAULT_HOMOGENOUS_COMPONENT);
		vector->w = w;
		lua_settop(L, 1);
		return 1;
	}

	int vector_get_truncated(lua_State * L) {
		glm::vec4 value = to_vec4(L, 1);
		float w = luaL_optnumber(L, 2, DEFAULT_HOMOGENOUS_COMPONENT);
		value.w = w;
		push_vector(L, value);
		return 1;
	}

	int vector_homogenize(lua_State * L) {
		glm::vec4 * vector = to_vector(L, 1);

		if (vector->w == 0) return luaL_error(L, "undefined for vector with homogenous coordinate of 0");
		else {
			*vector = *vector / vector->w;
			lua_settop(L, 1);
			return 1;
		}
	}

	int vector_get_homogenized(lua_State * L) {
		glm::vec4 value = to_vec4(L, 1);

		if (value.w == 0) {
			lua_pushnil(L); // -- undefined
			return 1; 
		} else {
			value = value / value.w;
			push_vector(L, value);
			return 1;
		}
	}
}
