#include "mesh_object.h"

#include "vertex.h"

#include <glm/gtc/matrix_inverse.hpp>

using namespace luagame;

luagame::mesh_object::mesh_object(luagame::material_cache * material_cache, luagame::texture_cache * texture_cache) :
	material_cache(material_cache),
	texture_cache(texture_cache),
	gl_buffer(NULL),
	dirty(false),
	vertices() {}

luagame::mesh_object::~mesh_object() {
	set_material(nullptr);
}

void luagame::mesh_object::clear() {
	if (vertices.size() > 0) {
		vertices.clear();
		dirty = true;
	}
}

void luagame::mesh_object::set(const vertex * const vertex_data, int count) {
	if (vertices.size() > 0) {
		vertices.clear();
		dirty = true;
	}

	if (count > 0) {
		vertices.reserve(count);
		vertices.insert(vertices.end(), vertex_data, vertex_data + count);
		dirty = true;
	}
}

void luagame::mesh_object::append(const vertex * vertex) {
	append(vertex, 1);
}

void luagame::mesh_object::append(const vertex * vertex_data, int count) {
	if (count > 0) {
		vertices.reserve(vertices.size() + count);
		vertices.insert(vertices.end(), vertex_data, vertex_data + count);
		dirty = true;
	}
}

std::shared_ptr<luagame::texture_object> luagame::mesh_object::get_texture() const  {
	return texture;
}

void luagame::mesh_object::set_texture(const char * filename) {
	texture = (*texture_cache)[filename];
}

void luagame::mesh_object::set_texture(std::shared_ptr<texture_object> new_texture) {
	texture = new_texture;
}

std::shared_ptr<material_object> luagame::mesh_object::get_material() const {
	return material;
}

void luagame::mesh_object::set_material(const luagame::material_object::material_options & mtlopts) {
	material = (*material_cache)[mtlopts];
}

void luagame::mesh_object::set_material(std::shared_ptr<luagame::material_object> new_material) {
	material = new_material;
}

void luagame::mesh_object::bind() {

	if (!gl_buffer) {
		glGenVertexArrays(1, &gl_vertex_array);
		glGenBuffers(1, &gl_buffer);
		dirty = true;
	}

	glBindVertexArray(gl_vertex_array);

	if (dirty) {

		glBindBuffer(GL_ARRAY_BUFFER, gl_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(luagame::vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		GLint position_attr = material->get_targets().position_attr;
		GLint color_attr = material->get_targets().color_attr;
		GLint normal_attr = material->get_targets().normal_attr;
		GLint texcoord_attr = material->get_targets().texcoord_attr;

		material_object::material_options mtlopts = material->opts();

		glEnableVertexAttribArray(position_attr);
		glVertexAttribPointer(position_attr, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(0 * sizeof(glm::vec3)));

		if (mtlopts.use_color) {
			glEnableVertexAttribArray(color_attr);
			glVertexAttribPointer(color_attr, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(1 * sizeof(glm::vec3)));
		}

		if (mtlopts.use_normal) {
			glEnableVertexAttribArray(normal_attr);
			glVertexAttribPointer(normal_attr, 3, GL_FLOAT, GL_TRUE, sizeof(vertex), (void*)(2 * sizeof(glm::vec3)));
		}

		if (mtlopts.use_texture) {
			glEnableVertexAttribArray(texcoord_attr);
			glVertexAttribPointer(texcoord_attr, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(3 * sizeof(glm::vec3)));
		}

		dirty = false;
	}

}

size_t luagame::mesh_object::size() { return vertices.size(); }

void luagame::mesh_object::draw(glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix, luagame::environment_object environment) {
	


	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	if (material->options.max_lights < environment.light_count()) {
		material_object::material_options opts = material->options;
		opts.max_lights = environment.light_count();
		set_material(std::make_shared<luagame::material_object>(opts));
	}

	material->bind();
	environment.bind(material, view_matrix);

	material_object::program_targets targets = material->get_targets();

	glUniformMatrix4fv(targets.model_uni, 1, GL_FALSE, (GLfloat *)(&model_matrix));
	glUniformMatrix4fv(targets.view_uni, 1, GL_FALSE, (GLfloat *)(&view_matrix));
	glUniformMatrix4fv(targets.proj_uni, 1, GL_FALSE, (GLfloat *)(&projection_matrix));

	if (targets.invtrans_uni != -1) {
		glm::mat4 invtrans_matrix = glm::inverseTranspose(view_matrix * model_matrix);
		glUniformMatrix4fv(targets.invtrans_uni, 1, GL_FALSE, (GLfloat *)(&invtrans_matrix));
	}

	if (material->opts().use_texture && texture) texture->bind();

	this->bind();

	glDrawArrays(GL_TRIANGLES, 0, this->size());

}
