#pragma once

#include "../common.h"
#include "graphics_math.h"

namespace luagame {

	struct vertex { 
		glm::vec3 position, color, normal;
		glm::vec2 texcoord;
	};

}