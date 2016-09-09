#pragma once

#include "../common.h"

#include "../graphics/graphics_math.h"

#include "scene_node.h"
#include "mesh_node.h"
#include "camera_node.h"

namespace luagame {
	class scene_manager : virtual public reference_counted {
	public:

		scene_manager() : root_node(this) {}

		virtual ~scene_manager() {}

	public:

		void add(scene_node * child) {
			root_node.add_child(child);
		}

		void remove_child(scene_node * child) {
			root_node.remove_child(child);
		}

		scene_node * root() {
			return &root_node;
		}
		
		camera_node * camera() {
			return camera_node;
		}

		void set_camera(camera_node * camera_node) {
			_log("scene camera is now: %p", camera_node);
			this->camera_node = camera_node;
		}

		void render() {
			render(&root_node);
		}

	private:

		void render(scene_node * node) {
			if (!camera_node) _err("no camera in scene");

			if (node->type() == node_type::mesh_node_type) {

				mesh_node * mesh_node = dynamic_cast<luagame::mesh_node *>(node);
				material * mtl = mesh_node->get_material();
				texture * tex = mesh_node->get_texture();

				glm::mat4 model = mesh_node->get_transform();
				glm::mat4 view = glm::inverse(camera_node->get_transform());
				glm::mat4 projection = camera_node->get_projection();

				mesh_node->get_material()->bind();

				material::program_targets targets = mtl->get_targets();
			
				glUniformMatrix4fv(targets.model_uni, 1, GL_FALSE, (GLfloat *)(&model));
				glUniformMatrix4fv(targets.view_uni, 1, GL_FALSE, (GLfloat *)(&view));
				glUniformMatrix4fv(targets.proj_uni, 1, GL_FALSE, (GLfloat *)(&projection));

				mesh_node->get_mesh()->bind();

				if (mtl->opts().use_texture && tex) tex->bind();
				
				glDrawArrays(GL_TRIANGLES, 0, mesh_node->get_mesh()->size());
			}

			for (scene_node_list::const_iterator it = node->get_children().begin(), end = node->get_children().end(); it != end; ++it)
				render(*it);
		}

	private:

		scene_node		root_node;
		camera_node *	camera_node;

	};
}