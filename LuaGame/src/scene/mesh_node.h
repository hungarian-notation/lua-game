#pragma once

#include "..\common.h"
#include "..\graphics\graphics_math.h"

#include "scene_node.h"

#include "..\graphics\mesh.h"
#include "..\graphics\material.h"
#include "..\graphics\texture.h"

#include <list>
#include <stdexcept>

namespace luagame {
	class mesh_node : virtual public scene_node {
	public:

		mesh_node(mesh * mesh) : mesh(mesh) {
			_notnull(mesh);
			this->mesh->acquire();
		}

		~mesh_node() override {
			this->mesh->release();
		}

		virtual node_type type() { return node_type::mesh_node_type; }

	public:

		mesh * get_mesh() { return mesh; }

		material * get_material() { return mesh->get_material(); }

		texture * get_texture() { return mesh->get_texture(); }

	private:

		mesh * mesh;

	};
}