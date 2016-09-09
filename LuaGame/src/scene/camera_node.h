#pragma once

#include "..\common.h"
#include "..\graphics\graphics_math.h"
#include "scene_node.h"

namespace luagame {
	class graphics_context;

	class camera_node : public virtual scene_node {
	public:

		camera_node(graphics_context * graphics);

	public:

		void ortho();

		void ortho(float width, float height);

		void ortho(float left, float right, float bottom, float top, float near, float far);

		void perspective(float fov, float near, float far);

		void set_projection(glm::mat4 projection);

		glm::mat4 get_projection() { return projection; }

		void update_transform() override;

	private:

		graphics_context * graphics;

		glm::mat4 projection;
		glm::mat4 view;

	};
}