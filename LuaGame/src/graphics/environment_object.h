#pragma once

#include "..\common.h"
#include "graphics_math.h"

#include <vector>

namespace luagame {

struct light {
	glm::vec4	position;
	glm::vec3	color;
	light(glm::vec3 direction, glm::vec3 color);
	light(glm::vec3 position, float radius, glm::vec3 color);
};

class environment_object {
public:
	environment_object();
	~environment_object();
	void add_light(light light);
	const std::vector<light> & get_lights() const;
private:
	std::vector<light> lights;

};

}