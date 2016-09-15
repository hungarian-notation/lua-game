#include "mesh.h"

#include "vertex.h"

luagame::mesh_object::mesh_object() :
	material	(nullptr),
	gl_buffer	(NULL),
	dirty		(false),
	vertices	(new std::vector<vertex>()) {}

luagame::mesh_object::~mesh_object() {
	set_material(nullptr);
	set_texture((luagame::texture *)nullptr);
	delete vertices;
}

void luagame::mesh_object::clear() {
	if (vertices->size() > 0) {
		vertices->clear();
		dirty = true;
	}
}

void luagame::mesh_object::set(const vertex * const vertex_data, int count) {
	if (vertices->size() > 0) {
		vertices->clear();
		dirty = true;
	}

	if (count > 0) {
		vertices->reserve(count);
		vertices->insert(vertices->end(), vertex_data, vertex_data + count);
		dirty = true;
	}
}

void luagame::mesh_object::append(const vertex * vertex) {
	append(vertex, 1);
}

void luagame::mesh_object::append(const vertex * vertex_data, int count) {
	if (count > 0) {
		vertices->reserve(vertices->size() + count);
		vertices->insert(vertices->end(), vertex_data, vertex_data + count);
		dirty = true;
	}
}

luagame::texture * luagame::mesh_object::get_texture() {
	return this->texture;
}

void luagame::mesh_object::set_texture(const char * filename) {
	if (this->texture) this->texture->release();
	this->texture = new luagame::texture(filename);
	this->texture->acquire();
}

void luagame::mesh_object::set_texture(luagame::texture * new_texture) {
	if (this->texture) this->texture->release();
	this->texture = new_texture;
	if (this->texture) this->texture->acquire();
}

luagame::material * luagame::mesh_object::get_material() {
	return this->material;
}

void luagame::mesh_object::set_material(const luagame::material::options & mtlopts) {
	if (this->material) this->material->release();
	this->material = new luagame::material(mtlopts);
	this->material->acquire();
}

void luagame::mesh_object::set_material(luagame::material * new_material) {
	if (this->material) this->material->release();
	this->material = new_material;
	if (this->material) this->material->acquire();
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(luagame::vertex) * vertices->size(), vertices->data(), GL_STATIC_DRAW);

		GLint position_attr = material->get_targets().position_attr;
		GLint color_attr = material->get_targets().color_attr;
		GLint normal_attr = material->get_targets().normal_attr;
		GLint texcoord_attr = material->get_targets().texcoord_attr;

		material::options mtlopts = material->opts();
		
		if (mtlopts.use_position) {
			glEnableVertexAttribArray(position_attr);
			glVertexAttribPointer(position_attr, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(0 * sizeof(glm::vec3)));
		}

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

size_t luagame::mesh_object::size() { return vertices->size(); }

void luagame::mesh_object::draw(glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix) {

	material->bind();

	material::program_targets targets = material->get_targets();

	glUniformMatrix4fv(targets.model_uni, 1, GL_FALSE, (GLfloat *)(&model_matrix));
	glUniformMatrix4fv(targets.view_uni, 1, GL_FALSE, (GLfloat *)(&view_matrix));
	glUniformMatrix4fv(targets.proj_uni, 1, GL_FALSE, (GLfloat *)(&projection_matrix));

	this->bind();

	if (material->opts().use_texture && texture) texture->bind();

	glDrawArrays(GL_TRIANGLES, 0, this->size());
}
