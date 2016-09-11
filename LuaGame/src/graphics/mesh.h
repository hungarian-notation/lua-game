#pragma once

#include "../common.h"

#include <vector>

#include "material.h"
#include "texture.h"

#include "../graphics/graphics_math.h"

namespace luagame {


	struct vertex; // so as to avoid including glm into this header

	class mesh : virtual public reference_counted {
	public:

		mesh();

		virtual ~mesh();

	public:

		void clear();

		void set(const vertex * const vertex_data, int count);

		void append(const vertex * vertex);

		void append(const vertex * const vertex_data, int count);

		luagame::texture * get_texture();

		void set_texture(const char * filename);

		void set_texture(luagame::texture * texture);

		luagame::material * get_material();

		void set_material(const luagame::material::options &mtlopts);

		void set_material(luagame::material * material);

		void bind();

		size_t size();

		void draw(glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix);

	private:

		luagame::material * material;

		luagame::texture * texture;

		std::vector<vertex> * vertices; // a pointer simply to avoid including glm into this header

		GLuint gl_buffer, gl_vertex_array;

		bool dirty;

	};
}