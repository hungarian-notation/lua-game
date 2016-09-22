#include "environment_object.h"

using namespace luagame;
using namespace glm;

void luagame::environment_object::bind(std::shared_ptr<luagame::material_object> material, glm::mat4 view_matrix) {
	material_object::program_targets targets = material->get_targets();

	glUniform3fv(targets.ambientlight_uni, 1, (GLfloat *)(&ambient_light));

	glm::vec4 * eyespace_positions	= new glm::vec4[material->options.max_lights];
	glm::vec3 * colors = new glm::vec3[material->options.max_lights];

	for (unsigned int i = 0; i < material->options.max_lights; i++) {
		if (i < light_count()) {
			float radius = light_positions[i].w;
			eyespace_positions[i] = vec4(vec3(view_matrix * vec4(vec3(light_positions[i]), radius == 0.0F ? 0.0F : 1.0F)), radius);
			colors[i] = light_colors[i];
		} else {
			eyespace_positions[i] = vec4(0, 0, 0, 0);
			colors[i] = vec3(0, 0, 0);
		}
	}

	glUniform4fv(targets.lightpos_uni, material->options.max_lights, (GLfloat*)(eyespace_positions));
	glUniform3fv(targets.lightcolor_uni, material->options.max_lights, (GLfloat*)(colors));

	delete[] eyespace_positions;
	delete[] colors;
}

luagame::light::light(glm::vec3 direction, glm::vec3 color) :
	position(glm::vec4(direction, 0)),
	color(color) {}

luagame::light::light(glm::vec3 position, float radius, glm::vec3 color) :
	position(glm::vec4(position, radius)),
	color(color) {}

luagame::environment_object::environment_object() : ambient_light(0.1F) {}

luagame::environment_object::~environment_object() {}

void luagame::environment_object::set_ambient(glm::vec3 ambient) {
	ambient_light = ambient;
}

glm::vec3 luagame::environment_object::get_ambient() {
	return ambient_light;
}

void luagame::environment_object::add_light(light light) {
	light_positions.push_back(light.position);
	light_colors.push_back(light.color);
}

unsigned luagame::environment_object::light_count() {
	return light_positions.size();
}
