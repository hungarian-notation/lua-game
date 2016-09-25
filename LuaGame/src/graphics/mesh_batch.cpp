#include "mesh_batch.h"

#include <glm/gtc/matrix_inverse.hpp>

#include <glm/gtx/io.hpp>
#include <iostream>


using namespace luagame;

using namespace glm;
using namespace std;

mesh_batch::texture_group * luagame::mesh_batch::material_group::get_texture_group(const shared_ptr<texture_object>& texture) {
	int tex_id;

	if (texture) tex_id = texture->id();
	else tex_id = -1;

	if (!texture_groups.count(tex_id))
		texture_groups[tex_id] = texture_group{ texture };
	return &texture_groups[tex_id];
}

luagame::mesh_batch::mesh_batch() : drawing(false) {}

luagame::mesh_batch::~mesh_batch() {}

void luagame::mesh_batch::begin(luagame::environment_object env, mat4 view, mat4 projection) {
	if (drawing) throw runtime_error("must flush batch before starting a new draw operation");
	else {
		drawing = true;

		view_matrix = view;
		projection_matrix = projection;
		environment = env;
	}
}

void luagame::mesh_batch::add_instance(const mesh_instance & instance) {
	if (!drawing) throw runtime_error("must call begin() before adding a mesh instance");

	auto material_options = instance.get_material()->options;

	if (material_options.use_lighting && material_options.max_lights < environment.light_count()) {

		// If the material is lit and the environment for this batch defines more lights
		// than it supports, upgrade the material for the mesh.

		// This can be a fairly expensive operation, but it shouldn't happen frequently enough
		// to offset the value of the simplicity we get from hiding this process.

		material_options.max_lights = environment.light_count();

		instance.mesh->set_material(material_options);

		// Mesh objects contain a reference to a shared material cache object. The set_material
		// call will only trigger the compilation of a new shader if the material's options
		// are currently unique.
	}

	get_material_group(instance.get_material())->
		get_texture_group(instance.get_texture())->
		instances.push_back(instance);
}

void luagame::mesh_batch::add_instance(shared_ptr<mesh_object> mesh, glm::mat4 model_matrix) {
	add_instance(mesh_instance(mesh, model_matrix));
}

void luagame::mesh_batch::flush() {

	if (!drawing) return; // TODO: Should this throw an exception?

	for (auto m_it = material_groups.begin(); m_it != material_groups.end(); m_it++) {

		material_group &group = m_it->second;
		auto material = group.material;

		if (material->options.use_depthtest) {
			glEnable(GL_DEPTH_TEST);
		} else {
			glDisable(GL_DEPTH_TEST);
		}

		glEnable(GL_CULL_FACE); // TODO: Should the material get a say?
		glCullFace(GL_BACK);
		
		if (material->options.use_alphamask) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		} else {
			glDisable(GL_BLEND);
		}

		material->bind();

		environment.bind(material, view_matrix);

		material_object::program_targets targets = material->get_targets();

		glUniformMatrix4fv(targets.view_uni, 1, GL_FALSE, (GLfloat *)(&view_matrix));
		glUniformMatrix4fv(targets.proj_uni, 1, GL_FALSE, (GLfloat *)(&projection_matrix));

		for (auto t_it = group.texture_groups.begin(); t_it != group.texture_groups.end(); t_it++) {
			texture_group &texture_group = t_it->second;

			auto texture = texture_group.texture;

			if (texture && material->options.use_texture) texture->bind();

			for (auto instance_it = texture_group.instances.begin(); instance_it != texture_group.instances.end(); instance_it++) {

				glUniformMatrix4fv(targets.model_uni, 1, GL_FALSE, (GLfloat *)(&instance_it->model_matrix));

				if (targets.invtrans_uni != -1) {
					glm::mat4 invtrans_matrix = glm::inverseTranspose(view_matrix * instance_it->model_matrix);
					glUniformMatrix4fv(targets.invtrans_uni, 1, GL_FALSE, (GLfloat *)(&invtrans_matrix));
				}

				instance_it->mesh->bind();

				glDrawArrays(GL_TRIANGLES, 0, instance_it->mesh->size());

			}
		}
	}

	material_groups.clear();

	drawing = false;
}

mesh_batch::material_group * luagame::mesh_batch::get_material_group(const shared_ptr<material_object>& material) {
	if (!material_groups.count(material->id()))
		material_groups[material->id()] = material_group{ material };
	return &material_groups[material->id()];
}
