
#include "lgapi_math.h"
#include <cstdlib>

#include <iostream>
#include <sstream>
#include <glm\gtx\io.hpp>

namespace {
	int meta_tostring(lua_State * L);
	int meta_multiply(lua_State * L);

	int set_identity(lua_State * L);
	int set_ortho(lua_State * L);
	int set_perspective(lua_State * L);

	int translate(lua_State * L);
	int get_translated(lua_State * L);

	int rotate(lua_State * L);
	int get_rotated(lua_State * L);

	int scale(lua_State * L);
	int get_scaled(lua_State * L);

	int invert(lua_State * L);
	int get_inverse(lua_State * L);

	int transpose(lua_State * L);
	int get_transposed(lua_State * L);

	int get_component(lua_State * L);
	int set_component(lua_State * L);

	int det(lua_State * L);
}

void luagame_pushmatrix(lua_State * L, glm::mat4 matrix) {
	glm::mat4 * udata = (glm::mat4*) lua_newuserdata(L, sizeof(glm::mat4));

	*udata = matrix;

	if (luaL_newmetatable(L, LUAGAME_MATRIX)) {

		luaL_Reg functions[] = {

			{ LUA_METAMETHOD_TOSTRING, &meta_tostring },

			{ LUA_METAMETHOD_MUL, &meta_multiply },

			{ "set_identity", &set_identity },
			{ "set_ortho", &set_ortho },
			{ "set_perspective", &set_perspective },

			{ "translate", &translate },
			{ "get_translated", &get_translated },

			{ "rotate", &rotate },
			{ "get_rotated", &get_rotated },

			{ "scale", &scale },
			{ "get_scaled", &get_scaled },

			{ "invert", &invert },
			{ "get_inverse", &get_inverse },

			{ "transpose", &transpose },
			{ "get_transpose", &get_transposed },

			{ "det", &det },

			{ "get", &get_component },
			{ "set", &set_component },

			{ NULL, NULL }
		};

		luaL_setfuncs(L, functions, NULL);

		lua_pushvalue(L, -1);
		lua_setfield(L, -1, LUA_METAMETHOD_INDEX);
	}

	lua_setmetatable(L, -2);
}

int luagame_ismatrix(lua_State * L, int idx) {
	return (luaL_testudata(L, idx, LUAGAME_MATRIX) != nullptr);
}

glm::mat4 * luagame_tomatrix(lua_State * L, int idx) {
	glm::mat4 * udata = (glm::mat4*) luaL_checkudata(L, idx, LUAGAME_MATRIX);
	if (udata)
		return udata;
	else {
		luaL_argerror(L, idx, "expected a matrix");
		return nullptr; // -- unreachable
	}
}

glm::mat4 luagame_tomat4(lua_State * L, int idx) {
	return *luagame_tomatrix(L, idx);
}

int lgapi_create_matrix(lua_State * L) {
	int args = lua_gettop(L);

	glm::mat4 matrix;

	switch (args) {
	case 0: // identity
		matrix = glm::mat4();
		break;
	case 1: // copy
		if (luagame_ismatrix(L, 1))
			matrix = *luagame_tomatrix(L, 1);
		else
			luaL_argerror(L, 1, "not a matrix");
		break;
	case 4: // column vectors
		matrix = glm::mat4(
			luagame_tovec4(L, 1),
			luagame_tovec4(L, 2),
			luagame_tovec4(L, 3),
			luagame_tovec4(L, 4));
		break;
	case 16: // column major components
		matrix = glm::mat4(
			luaL_checknumber(L, 1),
			luaL_checknumber(L, 2),
			luaL_checknumber(L, 3),
			luaL_checknumber(L, 4),
			luaL_checknumber(L, 5),
			luaL_checknumber(L, 6),
			luaL_checknumber(L, 7),
			luaL_checknumber(L, 8),
			luaL_checknumber(L, 9),
			luaL_checknumber(L, 10),
			luaL_checknumber(L, 11),
			luaL_checknumber(L, 12),
			luaL_checknumber(L, 13),
			luaL_checknumber(L, 14),
			luaL_checknumber(L, 15),
			luaL_checknumber(L, 16)
		);
		break;
	default:
		luaL_error(L, "expected 0 (identity), 1 (copy), 4 (column vectors), or 16 (column-major components) arguments");
	}

	luagame_pushmatrix(L, matrix);

	return 1;
}

namespace {
	int meta_tostring(lua_State * L) {
		glm::mat4 matrix = *luagame_tomatrix(L, 1);
		std::stringstream buffer;
		buffer << matrix;
		lua_pushstring(L, buffer.str().c_str());
		return 1;
	}

	int meta_multiply(lua_State * L) {

		if (luagame_ismatrix(L, 1) && luagame_ismatrix(L, 2)) {
			luagame_pushmatrix(L, luagame_tomat4(L, 1) * luagame_tomat4(L, 2));
		} else if (luagame_ismatrix(L, 1) && luagame_isvector(L, 2)) {
			luagame_pushvector(L, luagame_tomat4(L, 1) * luagame_tovec4(L, 2));
		} else {
			return luaL_error(L, "invalid operands for matrix multiplication");
		}

		return 1;

	}

	int set_identity(lua_State * L) {
		*luagame_tomatrix(L, 1) = glm::mat4();

		lua_settop(L, 1);
		return 1;
	}

	int set_ortho(lua_State * L) {
		glm::mat4 * matrix = luagame_tomatrix(L, 1);

		float left = (float)luaL_checknumber(L, 2);
		float right = (float)luaL_checknumber(L, 3);
		float bottom = (float)luaL_checknumber(L, 4);
		float top = (float)luaL_checknumber(L, 5);

		*matrix = glm::ortho(left, right, bottom, top);

		lua_settop(L, 1);
		return 1;
	}

	int set_perspective(lua_State * L) {
		glm::mat4 * matrix = luagame_tomatrix(L, 1);

		if (lua_gettop(L) < 6) {
			float fovy = (float)luaL_checknumber(L, 2);
			float aspect = (float)luaL_checknumber(L, 3);
			float near = (float)luaL_checknumber(L, 4);
			float far = (float)luaL_checknumber(L, 5);
			*matrix = glm::perspective(fovy, aspect, near, far);
		} else {
			float fov = (float)luaL_checknumber(L, 2);
			float width = (float)luaL_checknumber(L, 3);
			float height = (float)luaL_checknumber(L, 4);
			float near = (float)luaL_checknumber(L, 5);
			float far = (float)luaL_checknumber(L, 6);
			*matrix = glm::perspectiveFov(fov, width, height, near, far);
		}

		lua_settop(L, 1);
		return 1;
	}

	int translate(lua_State * L) {
		glm::mat4 * matrix = luagame_tomatrix(L, 1);
		glm::mat4 result;

		switch (lua_gettop(L)) {

		case 2:
		{
			glm::vec3 vector = luagame_tovec3(L, 2);
			result = glm::translate(*matrix, vector);
		}
		break;

		case 4:
		{
			glm::vec3 vector = glm::vec3((float)lua_tonumber(L, 2), (float)lua_tonumber(L, 3), (float)lua_tonumber(L, 4));
			result = glm::translate(*matrix, vector);
		}
		break;

		default:
			return luaL_error(L, "invalid number of arguments to matrix.translate");
		}

		*matrix = result;

		lua_settop(L, 1);
		return 1;
	}

	int get_translated(lua_State * L) {
		luagame_pushmatrix(L, luagame_tomat4(L, 1)); // -- push duplicate
		lua_copy(L, -1, 1); // -- copy it over the original matrix
		lua_pop(L, 1); // -- remove the initial copy
		return translate(L);  // -- call :translate on the new matrix
	}

	int scale(lua_State * L) {

		if (lua_isnumber(L, 2)) {
			glm::mat4 * matrix = luagame_tomatrix(L, 1);
			float scalar = (float)lua_tonumber(L, 2);
			*matrix = glm::scale(*matrix, glm::vec3(scalar, scalar, scalar));
		} else {
			glm::mat4 * matrix = luagame_tomatrix(L, 1);
			glm::vec3 vector = luagame_tovec3(L, 2);
			*matrix = glm::scale(*matrix, vector);
		}

		lua_settop(L, 1);
		return 1;
	}

	int get_scaled(lua_State * L) {
		luagame_pushmatrix(L, luagame_tomat4(L, 1)); // -- push duplicate
		lua_copy(L, -1, 1); // -- copy it over the original matrix
		lua_pop(L, 1); // -- remove the initial copy
		return scale(L);  // -- call :scale on the new matrix
	}

	int rotate(lua_State * L) {
		switch (lua_gettop(L)) {

		case 3:
		{
			glm::mat4 * matrix = luagame_tomatrix(L, 1);
			float angle = (float)lua_tonumber(L, 2);
			glm::vec3 axis = luagame_tovec3(L, 3);
			*matrix = glm::rotate(*matrix, angle, axis);
		}
		break;

		case 5:
		{
			glm::mat4 * matrix = luagame_tomatrix(L, 1);
			float angle = (float)lua_tonumber(L, 2);

			glm::vec3 axis = glm::vec3(
				(float)lua_tonumber(L, 3),
				(float)lua_tonumber(L, 4),
				(float)lua_tonumber(L, 5)
			);

			*matrix = glm::rotate(*matrix, angle, axis);
		}
		break;

		default:
			return luaL_error(L, "invalid number of arguments to matrix.rotate");
		}

		lua_settop(L, 1);
		return 1;
	}

	int get_rotated(lua_State * L) {
		luagame_pushmatrix(L, luagame_tomat4(L, 1)); // -- push duplicate
		lua_copy(L, -1, 1); // -- copy it over the original matrix
		lua_pop(L, 1); // -- remove the initial copy
		return rotate(L);  // -- call :rotate on the new matrix
	}

	int invert(lua_State * L) {
		glm::mat4 * matrix = luagame_tomatrix(L, 1);
		*matrix = glm::inverse(*matrix);
		lua_settop(L, 1);
		return 1;
	}

	int get_inverse(lua_State * L) {
		luagame_pushmatrix(L, glm::inverse(luagame_tomat4(L, 1)));
		return 1;
	}

	int transpose(lua_State * L) {
		glm::mat4 * matrix = luagame_tomatrix(L, 1);
		*matrix = glm::transpose(*matrix);
		lua_settop(L, 1);
		return 1;
	}

	int get_transposed(lua_State * L) {
		luagame_pushmatrix(L, glm::transpose(luagame_tomat4(L, 1)));
		return 1;
	}

	int get_component(lua_State * L) {
		glm::mat4 * matrix = luagame_tomatrix(L, 1);
		int col = (int)luaL_checkinteger(L, 2);
		int row = (int)luaL_checkinteger(L, 3);

		lua_pushnumber(L, (*matrix)[col - 1][row - 1]);

		return 1;
	}

	int set_component(lua_State * L) {
		glm::mat4 * matrix = luagame_tomatrix(L, 1);
		int col = (int)luaL_checkinteger(L, 2);
		int row = (int)luaL_checkinteger(L, 3);
		float number = (float)luaL_checknumber(L, 4);

		(*matrix)[col - 1][row - 1] = number;

		return 0;
	}

	int det(lua_State * L) {
		glm::mat4 * matrix = luagame_tomatrix(L, 1);
		lua_pushnumber(L, glm::determinant(*matrix));
		return 1;
	}
}