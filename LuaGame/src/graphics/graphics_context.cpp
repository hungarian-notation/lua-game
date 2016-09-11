#include "graphics_context.h"
#include "../common.h"

using namespace luagame;

luagame::graphics_context::graphics_context(int width, int height, std::string title) : title(title) {
	if (!glfwInit())
		_err("failed to initialize");
	else {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, LG_OPENGL_VERSION_MAJOR);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, LG_OPENGL_VERSION_MINOR);
		glfwWindowHint(GLFW_FOCUSED, true);

		glfw_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

		glfwMakeContextCurrent(glfw_window);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		if (!glfw_window) {
			_err("failed to create window");
			glfwTerminate();
		}

		glEnable(GL_DEPTH_TEST);

		_log("initialized glfw context");
	}
}

luagame::graphics_context::~graphics_context() {
	glfwTerminate();

	_log("terminated glfw");
}

void luagame::graphics_context::poll_events() {
	glfwPollEvents();
}

void luagame::graphics_context::swap_buffers() {
	glfwSwapBuffers(glfw_window);
}

void luagame::graphics_context::set_title(std::string new_title) {
	glfwSetWindowTitle(glfw_window, new_title.c_str());
	title = new_title;
	_log("set window title to: %s", new_title.c_str());
}

bool luagame::graphics_context::get_should_close() {
	return glfwWindowShouldClose(glfw_window) == GLFW_TRUE;
}

void luagame::graphics_context::set_should_close(bool should_close) {
	glfwSetWindowShouldClose(glfw_window, should_close);
}