#pragma once

#include "lgapi_math.h"

namespace luagame {
	void push_vector_constructor(lua_State * L);

	void push_vector(lua_State * L, glm::vec4 vector);
	void push_vector(lua_State * L, glm::vec3 vector, float w = 1);

	int is_vector(lua_State * L, int idx);

	glm::vec4 * to_vector(lua_State * L, int idx);

	glm::vec4 to_vec4(lua_State * L, int idx);
	glm::vec3 to_vec3(lua_State * L, int idx);

	namespace api {
		int create_vector(lua_State * L);
	}
}
