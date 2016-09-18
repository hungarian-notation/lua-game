#pragma once

#include "common.h"
#include "graphics/graphics_math.h"

#include <map>

namespace luagame {


	class window_context {

	public:

		window_context(int width = 800, int height = 600, std::string title = "luagame");

		virtual ~window_context();

	public:

		enum cursor_mode { normal_cursor, hidden_cursor, captured_cursor };

	public:

		void poll_events();

		void clear() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		glm::ivec2 get_size() {
			glm::ivec2 window_size;
			glfwGetFramebufferSize(glfw_window, &window_size.x, &window_size.y);
			return window_size;
		}

		void resize(int width, int height) {
			glfwSetWindowSize(glfw_window, width, height);
		}

		void swap_buffers();

		std::string get_title() { return title; }

		void set_title(const std::string new_title);

		void set_cursor_mode(cursor_mode mode);

		bool get_should_close();

		void set_should_close(bool should_close);

		void reset_viewport();

		glm::ivec4 get_viewport();

		void set_viewport(glm::ivec4 viewport);

		GLFWwindow * as_glfw() { return glfw_window; }

	private:

		GLFWwindow *	glfw_window;

		std::string		title;

		glm::ivec4		viewport;

	};
}