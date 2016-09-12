#pragma once

#include "../common.h"

#include "../scene_graph/scene_manager.h"
#include "texture.h"

namespace luagame {
	class graphics_context : virtual public reference_counted {
	public:

		graphics_context(int width = 800, int height = 600, std::string title = "luagame");

		virtual ~graphics_context();

	public:

		void poll_events();

		void clear() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		glm::ivec2 get_size() {
			glm::ivec2 window_size;
			glfwGetWindowSize(glfw_window, &window_size.x, &window_size.y);
			return window_size;
		}

		void resize(int width, int height) {
			glfwSetWindowSize(glfw_window, width, height);
		}

		void swap_buffers();

		std::string get_title() { return title; }

		void set_title(const std::string new_title);

		bool get_should_close();

		void set_should_close(bool should_close);

	private:

		GLFWwindow *	glfw_window;
		std::string		title;

	};
}