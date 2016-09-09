#include "common.h"

#include "graphics\graphics_context.h"
#include "graphics\mesh.h"

#include "scene_graph\scene_manager.h"
#include "scene_graph\scene_node.h"
#include "scene_graph\mesh_node.h"
#include "scene_graph\camera_node.h"

#include <glm/gtx/io.hpp>

#include <cstdlib>

#include "graphics\shader_loader.h"
#include "graphics\vertex.h"

#include "binding\binding.h"
#include "util\any_key.h"

using namespace luagame;

float randf() {
	return rand() % RAND_MAX / float(RAND_MAX);
}

int main() {
	int result = binding_execute();
	util::any_key();
	return result;
}

int old_main() {
	srand(time(NULL));

	luagame::graphics_context * graphics = new luagame::graphics_context();

	graphics->set_title("This is my title!");

	luagame::scene_graph * scene = new luagame::scene_graph(graphics);

	luagame::mesh * mesh = new luagame::mesh;

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

	scene_node	camera_pivot;
	camera_node camera(graphics);

	camera_pivot.add_child(&camera);

	scene->add(&camera_pivot);

	camera.perspective(glm::half_pi<float>(), 1, 100);
	camera.set_translation(glm::vec3(0, 0, 30));

#define MODEL_COUNT 1000

	mesh_node * models[MODEL_COUNT];

	for (int i = 0; i < MODEL_COUNT; i++) {
		models[i] = new mesh_node(mesh);
		models[i]->set_translation(glm::vec3(randf() * 30 - 15, randf() * 30 - 15, randf() * 30 - 15));
		models[i]->set_rotation(glm::vec3(randf() * glm::two_pi<float>(), randf() * glm::two_pi<float>(), randf()  * glm::two_pi<float>()));

		scene->add(models[i]);
	}

	mesh->release();
	scene->set_camera(&camera);

	float rot = 0.01;

	while (!graphics->get_should_close()) {
		graphics->clear();

		camera_pivot.rotate(glm::quat(glm::vec3(0.01, 0.001532, 0.01290872)));

		scene->render();

		graphics->swap_buffers();
		graphics->poll_events();
	}

	for (int i = 0; i < MODEL_COUNT; i++) {
		models[i]->release();
	}

	scene->release();
	graphics->release();

	return 0;
}