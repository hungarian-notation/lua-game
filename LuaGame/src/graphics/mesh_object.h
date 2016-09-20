#pragma once

#include "../common.h"
#include "../shared_cache.h"

#include "graphics_math.h"

#include "texture_object.h"
#include "material_object.h"
#include "vertex.h"
#include "environment_object.h"

#include <vector>

namespace luagame {
	class mesh_object {

	public:

		mesh_object();

		virtual ~mesh_object();

	public:

		void clear();

		void set(const vertex * const vertex_data, int count);

		void append(const vertex * vertex);

		void append(const vertex * const vertex_data, int count);

		std::shared_ptr<texture_object> get_texture();

		void set_texture(const char * filename, luagame::shared_cache<const char *, luagame::texture_object> &cache);

		void set_texture(std::shared_ptr<texture_object> texture);

		std::shared_ptr<material_object> get_material();

		void set_material(const luagame::material_object::options &mtlopts, luagame::shared_cache<material_object::options, material_object> &cache);

		void set_material(std::shared_ptr<material_object> material_object);

		void bind();

		size_t size();
		
		void draw(glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix, luagame::environment_object environment = luagame::environment_object());

	private:

		std::shared_ptr<material_object>	material;
		std::shared_ptr<texture_object>		texture;
		std::vector<vertex>					vertices;
		GLuint								gl_buffer, gl_vertex_array;
		bool								dirty;

	};
}