#include "common.h"

#include "graphics\window_context.h"
#include "graphics\mesh.h"

#include "scene_graph\scene_manager.h"
#include "scene_graph\scene_node.h"
#include "scene_graph\mesh_node.h"
#include "scene_graph\camera_node.h"

#include <glm/gtx/io.hpp>

#include <cstdlib>
#include <string>

#include "graphics\shader_loader.h"
#include "graphics\vertex.h"

#include "util\any_key.h"

#include "binding\lgapi.h"

using namespace luagame;

float randf() {
	return rand() % RAND_MAX / float(RAND_MAX);
}

int old_main();

int main() {
	int result = 0;

	{
		auto window(luagame::reference<window_context>(new luagame::window_context()));

		auto tex_reference = new luagame::reference<luagame::texture>(new luagame::texture("img_test.png"));
		auto same_reference = new luagame::weak_reference<luagame::texture>(tex_reference->get());

		delete same_reference;
		delete tex_reference;
	}

	// old_main();

	// result = luagame_execute();
	util::any_key();
	return result;
}

int old_main() {
	srand(time(NULL));

	luagame::window_context * graphics = new luagame::window_context();

	graphics->set_title("This is my title!");

	// luagame::scene_graph * scene = new luagame::scene_graph(graphics);

	luagame::mesh_object * mesh = new luagame::mesh_object;

	luagame::vertex vertices[4];

	vertices[0].position = glm::vec3(-1, -1, 0);
	vertices[0].color = glm::vec3(1, 1, 1);
	vertices[0].texcoord = glm::vec2(0, 0);

	vertices[1].position = glm::vec3(1, -1, 0);
	vertices[1].color = glm::vec3(1, 0, 0);
	vertices[1].texcoord = glm::vec2(1, 0);

	vertices[2].position = glm::vec3(1, 1, 0);
	vertices[2].color = glm::vec3(0, 1, 0);
	vertices[2].texcoord = glm::vec2(1, 1);

	vertices[3].position = glm::vec3(-1, 1, 0);
	vertices[3].color = glm::vec3(0, 0, 1);
	vertices[3].texcoord = glm::vec2(0, 1);

	mesh->append(&vertices[0]);
	mesh->append(&vertices[1]);
	mesh->append(&vertices[2]);
	mesh->append(&vertices[2]);
	mesh->append(&vertices[3]);
	mesh->append(&vertices[0]);

	material::options mtlopts = {};

	mtlopts.use_position = true;
	mtlopts.use_texture = true;
	mtlopts.use_transparency = true;

	mesh->set_material(mtlopts);
	mesh->set_texture("img_test.png");

	glm::mat4 view, projection;

	projection = glm::perspectiveFov(glm::half_pi<float>(), (float) graphics->get_size().x, (float) graphics->get_size().y, 1.0F, 100.0F);


	// scene_node	camera_pivot;
	// camera_node camera(graphics);

	// camera_pivot.add_child(&camera);

	// scene->add(&camera_pivot);

	// camera.perspective(glm::half_pi<float>(), 1, 100);
	// camera.set_translation(glm::vec3(0, 0, 30));

#define MODEL_COUNT 1000

	struct model {
		luagame::mesh_object * mesh;
		glm::mat4 transform;
	};

	model models[MODEL_COUNT];

	for (int i = 0; i < MODEL_COUNT; i++) {
		models[i].mesh = mesh;
		models[i].transform = glm::mat4();

		models[i].transform = glm::translate(
			models[i].transform,
			glm::vec3(randf() * 10 - 5, randf() * 10 - 5, randf() * 10 - 5)
		);

		float angle = glm::two_pi<float>() * randf();

		glm::vec3 axis = glm::normalize(glm::vec3(randf() * 2 - 1, randf() * 2 - 1, randf() * 2 - 1));

		glm::quat rotation(angle, axis);

		models[i].transform *= glm::mat4_cast(glm::normalize(rotation));

		models[i].transform = glm::scale(models[i].transform, glm::vec3(0.5F));
	}

	float rot = 0.01F;

	glm::quat viewrot;

	while (!graphics->get_should_close()) {
		graphics->clear();

		viewrot = glm::normalize(viewrot * glm::quat(glm::vec3(0.01F, 0.005F, 0.007F)));

		view = glm::mat4();

		view = glm::translate(view, glm::vec3(0, 0, -10));
		view = view * glm::mat4_cast(viewrot);

		for (int i = 0; i < MODEL_COUNT; i++) {
			models[i].mesh->draw(models[i].transform, view, projection);
		}

		graphics->swap_buffers();
		graphics->poll_events();
	}


	mesh->release();
	graphics->release();

	return 0;
}