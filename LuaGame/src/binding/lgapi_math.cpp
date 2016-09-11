#include "lgapi_math.h"

#define LUAGAME_MATRIX "luagame_matrix"

#include <iostream>
#include <glm\gtx\io.hpp>

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
		matrix = glm::mat4(check_vec4(L, 1), 
			check_vec4(L, 2), 
			check_vec4(L, 3), 
			check_vec4(L, 4));
		break;
	case 16: // column major components
		luaL_error(L, "component constructor not implemented");
		break;
	default:
		luaL_error(L, "expected 0 (identity), 1 (copy), 4 (column vectors), or 16 (column-major components) arguments");
	}

	push_matrix(L, matrix);

	return 1;
}

int luagame::api::matrix_set_identity(lua_State * L) {
	*check_matrix(L, 1) = glm::mat4();

	lua_settop(L, 1);
	return 1;
}

int luagame::api::matrix_set_ortho(lua_State * L) {
	glm::mat4 * matrix = check_matrix(L, 1);

	float left = luaL_checknumber(L, 2);
	float right = luaL_checknumber(L, 3);
	float bottom = luaL_checknumber(L, 4);
	float top = luaL_checknumber(L, 5);

	*matrix = glm::ortho(left, right, bottom, top);

	lua_settop(L, 1);
	return 1;
}

int luagame::api::matrix_set_perspective(lua_State * L) {
	glm::mat4 * matrix = check_matrix(L, 1);

	if (lua_gettop(L) < 6) {
		float fovy = luaL_checknumber(L, 2);
		float aspect = luaL_checknumber(L, 3);
		float near = luaL_checknumber(L, 4);
		float far = luaL_checknumber(L, 5);
		*matrix = glm::perspective(fovy, aspect, near, far);
	} else {
		float fov = luaL_checknumber(L, 2);
		float width = luaL_checknumber(L, 3);
		float height = luaL_checknumber(L, 4);
		float near = luaL_checknumber(L, 5);
		float far = luaL_checknumber(L, 6);
		*matrix = glm::perspectiveFov(fov, width, height, near, far);
	}

	lua_settop(L, 1);
	return 1;
}

int luagame::api::matrix_translate(lua_State * L) {
	glm::mat4 * matrix = check_matrix(L, 1);
	glm::vec3 translation = check_vec3(L, 2);
	*matrix = glm::translate(*matrix, translation);
}

int luagame::api::matrix_scale(lua_State * L) {
	return 0;
}

int luagame::api::matrix_rotate(lua_State * L) {
	return 0;
}

void luagame::push_matrix(lua_State * L, glm::mat4 matrix) {
	glm::mat4 * udata = (glm::mat4*) lua_newuserdata(L, sizeof(glm::mat4));

	*udata = matrix;

	if (luaL_newmetatable(L, LUAGAME_MATRIX)) {
		luaL_Reg functions[] = {

			{ "set_identity", &api::matrix_set_identity },
			{ "set_ortho", &api::matrix_set_ortho },
			{ "set_perspective", &api::matrix_set_perspective },

			{ "translate", &api::matrix_translate },
			{ "rotate", &api::matrix_rotate },
			{ "scale", &api::matrix_scale },

			{ NULL, NULL }
		};

		luaL_setfuncs(L, functions, NULL);
		lua_pushvalue(L, -1);
		lua_setfield(L, -1, LUA_METAMETHOD_INDEX);
	}

	lua_setmetatable(L, -2);

	std::cout << "pushed matrix: " << std::endl << (*udata) << std::endl;
}

int luagame::is_matrix(lua_State * L, int idx) {
	return (luaL_checkudata(L, idx, LUAGAME_MATRIX) != nullptr);
}

glm::mat4 * luagame::to_matrix(lua_State * L, int idx) {
	return check_matrix(L, idx);
}

glm::mat4 * luagame::check_matrix(lua_State * L, int idx) {
	glm::mat4 * udata = (glm::mat4*) luaL_checkudata(L, idx, LUAGAME_MATRIX);
	if (udata)
		return udata;
	else luaL_argerror(L, idx, "expected a matrix");
}

static bool is_vecn(lua_State * L, int idx, int comps) {
	if (lua_istable(L, idx)) {
		lua_len(L, idx);
		int len = lua_tonumber(L, -1);
		return len == comps;
	} else return false;
}

static lua_Number get_component(lua_State * L, int idx, int component) {
	lua_geti(L, idx, component);
	lua_Number value = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return value;
}

int luagame::is_vec3(lua_State * L, int idx) {
	return is_vecn(L, idx, 3);
}

void luagame::push_vec3(lua_State * L, glm::vec3 vec) {
	lua_createtable(L, 3, 0);
	lua_pushnumber(L, vec.x);
	lua_seti(L, -2, 1);
	lua_pushnumber(L, vec.y);
	lua_seti(L, -2, 2);
	lua_pushnumber(L, vec.z);
	lua_seti(L, -2, 3);
}

glm::vec3 luagame::to_vec3(lua_State * L, int idx) {
	float x = get_component(L, idx, 1);
	float y = get_component(L, idx, 2);
	float z = get_component(L, idx, 3);
	return glm::vec3(x, y, z);
}

glm::vec3 luagame::check_vec3(lua_State * L, int idx) {
	if (is_vec3(L, idx))
		return to_vec3(L, idx);
	else luaL_argerror(L, idx, "expected a 3-component vector");
}

int luagame::is_vec4(lua_State * L, int idx) {
	return is_vecn(L, idx, 4);
}


void luagame::push_vec4(lua_State * L, glm::vec4 vec) {
	lua_createtable(L, 4, 0);
	lua_pushnumber(L, vec.x);
	lua_seti(L, -2, 1);
	lua_pushnumber(L, vec.y);
	lua_seti(L, -2, 2);
	lua_pushnumber(L, vec.z);
	lua_seti(L, -2, 3);
	lua_pushnumber(L, vec.w);
	lua_seti(L, -2, 4);
}

glm::vec4 luagame::to_vec4(lua_State * L, int idx) {
	float x = get_component(L, idx, 1);
	float y = get_component(L, idx, 2);
	float z = get_component(L, idx, 3);
	float w = get_component(L, idx, 4);
	return glm::vec4(x, y, z, w);
}

glm::vec4 luagame::check_vec4(lua_State * L, int idx) {
	if (is_vec4(L, idx))
		return to_vec4(L, idx);
	else luaL_argerror(L, idx, "expected a 4-component vector");
}