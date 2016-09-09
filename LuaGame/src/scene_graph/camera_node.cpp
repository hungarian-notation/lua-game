#include "camera_node.h"

#include "..\graphics\graphics_context.h"

#include "scene_manager.h"

luagame::camera_node::camera_node(graphics_context * graphics) : graphics(graphics) {
	ortho();
}

void luagame::camera_node::ortho() {
	glm::ivec2 window_size = graphics->get_size();
	ortho((float)window_size.x, (float)window_size.y);
}

void luagame::camera_node::ortho(float width, float height) {
	ortho(0.0F, width, 0.0F, height, -1.0F, 100.0F);
}

void luagame::camera_node::ortho(float left, float right, float bottom, float top, float near, float far) {
	set_projection(glm::ortho(left, right, bottom, top, near, far));
}

void luagame::camera_node::perspective(float fov, float near, float far) {
	glm::ivec2 window_size = graphics->get_size();
	set_projection(glm::perspectiveFovRH(fov, (float)window_size.x, (float)window_size.y, near, far));
}

void luagame::camera_node::set_projection(glm::mat4 projection) {
	this->projection = projection;
	invalidate_transform();
}

void luagame::camera_node::update_transform() {
	scene_node::update_transform();
}
