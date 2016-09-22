#pragma once

#include "..\common.h"
#include "graphics_math.h"
#include "material_object.h"

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

	environment_object(glm::vec3 ambient_light) : ambient_light(ambient_light) {}

	~environment_object();

	void set_ambient(glm::vec3 ambient);

	glm::vec3 get_ambient();

	void add_light(light light);

	unsigned light_count();

	void bind(std::shared_ptr<luagame::material_object> material, glm::mat4 view_matrix);

private:
	std::vector<glm::vec4> light_positions;
	std::vector<glm::vec3> light_colors;
	glm::vec3 ambient_light;
};

}