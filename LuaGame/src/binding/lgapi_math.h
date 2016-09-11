#pragma once

#include "lgapi_shared.h"
#include "..\graphics\graphics_math.h"

namespace luagame {
	namespace api {
		int create_matrix(lua_State * L);

		int matrix_set_identity(lua_State * L);
		int matrix_set_ortho(lua_State * L);
		int matrix_set_perspective(lua_State * L);

		int matrix_translate(lua_State * L);
		int matrix_scale(lua_State * L);
		int matrix_rotate(lua_State * L);
	}

	int is_matrix(lua_State * L, int idx);
	glm::mat4 * check_matrix(lua_State * L, int idx);
	glm::mat4 * to_matrix(lua_State * L, int idx);
	void push_matrix(lua_State * L, glm::mat4 matrix);

	glm::vec3 to_vec3(lua_State * L, int idx);
	glm::vec3 check_vec3(lua_State * L, int idx);
	int is_vec3(lua_State * L, int idx);
	void push_vec3(lua_State * L, glm::vec3 vec);

	glm::vec4 to_vec4(lua_State * L, int idx);
	glm::vec4 check_vec4(lua_State * L, int idx);
	int is_vec4(lua_State * L, int idx);
	void push_vec4(lua_State * L, glm::vec4 vec);
}