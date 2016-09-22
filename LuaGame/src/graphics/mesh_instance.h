#pragma once

#include "mesh_object.h"
#include "graphics_math.h"

namespace luagame {

struct mesh_instance {
	std::shared_ptr<mesh_object>	mesh;
	glm::mat4						model_matrix;

	mesh_instance() : mesh(), model_matrix(1.0F) {}
	mesh_instance(std::shared_ptr<mesh_object> mesh, glm::mat4 model_matrix) : mesh(mesh), model_matrix(model_matrix) {}

	std::shared_ptr<material_object> get_material() const { return mesh->get_material(); }
	std::shared_ptr<texture_object> get_texture() const { return mesh->get_texture(); }
};

}