#include "lgapi_matrix.h"
#include <cstdlib>

using namespace luagame;
using namespace luagame::api;

#include <iostream>
#include <sstream>
#include <glm\gtx\io.hpp>

namespace {
	int matrix_tostring(lua_State * L);

	int matrix_multiply(lua_State * L);

	int matrix_set_identity(lua_State * L);
	int matrix_set_ortho(lua_State * L);
	int matrix_set_perspective(lua_State * L);

	int matrix_translate(lua_State * L);
	int matrix_scale(lua_State * L);
	int matrix_rotate(lua_State * L);
}

void luagame::push_matrix(lua_State * L, glm::mat4 matrix) {
	glm::mat4 * udata = (glm::mat4*) lua_newuserdata(L, sizeof(glm::mat4));

	*udata = matrix;

	if (luaL_newmetatable(L, LUAGAME_MATRIX)) {

		luaL_Reg functions[] = {

			{ LUA_METAMETHOD_TOSTRING, &matrix_tostring },

			{ LUA_METAMETHOD_MUL, &matrix_multiply },

			{ "set_identity", &matrix_set_identity },
			{ "set_ortho", &matrix_set_ortho },
			{ "set_perspective", &matrix_set_perspective },

			{ "translate", &matrix_translate },
			{ "rotate", &matrix_rotate },
			{ "scale", &matrix_scale },

			{ NULL, NULL }
		};

		luaL_setfuncs(L, functions, NULL);

		lua_pushvalue(L, -1);
		lua_setfield(L, -1, LUA_METAMETHOD_INDEX);
	}

	lua_setmetatable(L, -2);
}

int luagame::is_matrix(lua_State * L, int idx) {
	return (luaL_testudata(L, idx, LUAGAME_MATRIX) != nullptr);
}

glm::mat4 * luagame::to_matrix(lua_State * L, int idx) {
	glm::mat4 * udata = (glm::mat4*) luaL_checkudata(L, idx, LUAGAME_MATRIX);
	if (udata)
		return udata;
	else {
		luaL_argerror(L, idx, "expected a matrix");
		return nullptr; // -- unreachable
	}
}

glm::mat4 luagame::to_mat4(lua_State * L, int idx) {
	return *to_matrix(L, idx);
}

int luagame::api::create_matrix(lua_State * L) {
	int args = lua_gettop(L);

	glm::mat4 matrix;

	switch (args) {
	case 0: // identity
		matrix = glm::mat4();
		break;
	case 1: // copy
		if (is_matrix(L, 1))
			matrix = *to_matrix(L, 1);
		else
			luaL_argerror(L, 1, "not a matrix");
		break;
	case 4: // column vectors
		matrix = glm::mat4(
			to_vec4(L, 1),
			to_vec4(L, 2),
			to_vec4(L, 3),
			to_vec4(L, 4));
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

	push_matrix(L, matrix);

	return 1;
}

namespace {
	int matrix_tostring(lua_State * L) {
		glm::mat4 matrix = *to_matrix(L, 1);
		std::stringstream buffer;
		buffer << matrix;
		lua_pushstring(L, buffer.str().c_str());
		return 1;
	}

	int matrix_multiply(lua_State * L) {

		if (is_matrix(L, 1) && is_matrix(L, 2)) {
			push_matrix(L, to_mat4(L, 1) * to_mat4(L, 2));
		} else if (is_matrix(L, 1) && is_vector(L, 2)) {
			push_vector(L, to_mat4(L, 1) * to_vec4(L, 2));
		} else {
			return luaL_error(L, "invalid operands for matrix multiplication");
		}

		return 1;

	}

	int matrix_set_identity(lua_State * L) {
		*to_matrix(L, 1) = glm::mat4();

		lua_settop(L, 1);
		return 1;
	}

	int matrix_set_ortho(lua_State * L) {
		glm::mat4 * matrix = to_matrix(L, 1);

		float left = (float)luaL_checknumber(L, 2);
		float right = (float)luaL_checknumber(L, 3);
		float bottom = (float)luaL_checknumber(L, 4);
		float top = (float)luaL_checknumber(L, 5);

		*matrix = glm::ortho(left, right, bottom, top);

		lua_settop(L, 1);
		return 1;
	}

	int matrix_set_perspective(lua_State * L) {
		glm::mat4 * matrix = to_matrix(L, 1);

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

	int matrix_translate(lua_State * L) {
		glm::mat4 * matrix = to_matrix(L, 1);
		glm::mat4 result;

		switch (lua_gettop(L)) {
		
		case 2:
		{
			glm::vec3 vector = to_vec3(L, 2);
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

	int matrix_scale(lua_State * L) {

		if (lua_isnumber(L, 2)) {
			glm::mat4 * matrix = to_matrix(L, 1);
			float scalar = (float) lua_tonumber(L, 2);
			*matrix = glm::scale(*matrix, glm::vec3(scalar, scalar, scalar));
		} else {
			glm::mat4 * matrix = to_matrix(L, 1);
			glm::vec3 vector = to_vec3(L, 2);
			*matrix = glm::scale(*matrix, vector);
		}

		lua_settop(L, 1);
		return 1;
	}

	int matrix_rotate(lua_State * L) {
		switch (lua_gettop(L)) {

		case 3: 
		{
			glm::mat4 * matrix = to_matrix(L, 1);
			float angle = (float)lua_tonumber(L, 2);
			glm::vec3 axis = to_vec3(L, 3);
			*matrix = glm::rotate(*matrix, angle, axis);
		} 
		break;

		case 5:
		{
			glm::mat4 * matrix = to_matrix(L, 1);
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
}