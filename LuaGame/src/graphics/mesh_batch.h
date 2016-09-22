#pragma once

#include "..\common.h"

#include "mesh_instance.h"

#include "mesh_object.h"
#include "material_object.h"
#include "environment_object.h"

#include <map>


namespace luagame {

class mesh_batch {
private:

	struct texture_group {

		std::shared_ptr<texture_object> texture;
		std::vector<mesh_instance> instances;

	};

	struct material_group {

		std::shared_ptr<material_object> material;
		std::map<int, texture_group> texture_groups;

		texture_group * get_texture_group(const std::shared_ptr<texture_object> &texture);

	};

public:

	mesh_batch();
	~mesh_batch();

public:

	void begin(luagame::environment_object environment, glm::mat4 view_matrix, glm::mat4 projection_matrix);

	void add_instance(const mesh_instance &instance);

	void add_instance(std::shared_ptr<mesh_object> mesh, glm::mat4 model_matrix);

	void flush();

private:

	material_group * get_material_group(const std::shared_ptr<material_object> &material);

private:

	bool								drawing;

	glm::mat4							view_matrix, projection_matrix;
	luagame::environment_object			environment;

	std::map<GLuint, material_group>	material_groups;

};

}