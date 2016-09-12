#pragma once

#include "lgapi_math.h"

namespace luagame {
	void push_matrix_constructor(lua_State * L);
	void push_matrix(lua_State * L, glm::mat4 matrix);

	int is_matrix(lua_State * L, int idx);
	glm::mat4 * to_matrix(lua_State * L, int idx);
	glm::mat4 to_mat4(lua_State * L, int idx);

	namespace api {
		int create_matrix(lua_State * L);
	}
}