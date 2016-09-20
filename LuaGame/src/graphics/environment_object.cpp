#include "environment_object.h"

using namespace luagame;

const std::vector<light> & const luagame::environment_object::get_lights() const {
	return lights;
}

luagame::light::light(glm::vec3 direction, glm::vec3 color) :
	position(glm::vec4(direction, 0)),
	color(color) {}

luagame::light::light(glm::vec3 position, float radius, glm::vec3 color) :
	position(glm::vec4(position, radius)),
	color(color) {}

luagame::environment_object::environment_object() {}

luagame::environment_object::~environment_object() {}

void luagame::environment_object::add_light(light light) {
	lights.push_back(light);
}