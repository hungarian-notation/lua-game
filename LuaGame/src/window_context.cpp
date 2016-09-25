#include "window_context.h"
#include "common.h"

using namespace luagame;

// This is horrible. This function should be taken out back and shot. This is a terrible violation of this framework's design... but it works for now.
// The truth of the matter is that this framework is always going to have static state. There's nothing we can do to avoid that.

// All these callbacks could be sent to a (thread-local?) handler that maps GLFWwindow pointers to weak_ptr<window_context>, but that handler
// would only ever have one GLFWwindow and one window_context for my use-case.

// This is a global-state framework with a thin veneer of OOP. 

static luagame::window_context * active_window;

static void on_any_window_resize(GLFWwindow * window, int width, int height) {
	if (window == active_window->as_glfw())
		active_window->reset_viewport();
}

luagame::window_context::window_context(int width, int height, std::string title) : title(title) {

	if (!glfwInit())
		_err("failed to initialize");
	else {

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, LG_OPENGL_VERSION_MAJOR);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, LG_OPENGL_VERSION_MINOR);
		glfwWindowHint(GLFW_FOCUSED, true);
		glfwWindowHint(GLFW_SAMPLES, 4);

		// glfwWindowHint(GLFW_RESIZABLE, false);

		glfw_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

		glfwMakeContextCurrent(glfw_window);
		glfwSetWindowSizeCallback(glfw_window, on_any_window_resize);

		if (active_window) {
			throw std::runtime_error("window already exists");
		} else active_window = this;

		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		if (!glfw_window) {
			_err("failed to create window");
			glfwTerminate();
		}

		glEnable(GL_MULTISAMPLE);
		reset_viewport();


		_log("initialized glfw context");
	}
}

luagame::window_context::~window_context() {
	glfwTerminate();

	active_window = nullptr;

	_log("terminated glfw");
}

void luagame::window_context::poll_events() {
	glfwPollEvents();
}

void luagame::window_context::swap_buffers() {
	glfwSwapBuffers(glfw_window);
}

void luagame::window_context::set_title(std::string new_title) {
	glfwSetWindowTitle(glfw_window, new_title.c_str());
	title = new_title;
	_log("set window title to: %s", new_title.c_str());
}

void luagame::window_context::set_cursor_mode(cursor_mode mode) {
	switch (mode) {
	case normal_cursor:
		glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case hidden_cursor:
		glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		break;
	case captured_cursor:
		glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
	}

}

bool luagame::window_context::get_should_close() {
	return glfwWindowShouldClose(glfw_window) == GLFW_TRUE;
}

void luagame::window_context::set_should_close(bool should_close) {
	glfwSetWindowShouldClose(glfw_window, should_close);
}

void luagame::window_context::reset_viewport() {
	glm::ivec2 framesize = get_size();
	set_viewport((glm::ivec4) glm::vec4(0, 0, framesize.x, framesize.y));
}

glm::ivec4 luagame::window_context::get_viewport() {
	return this->viewport;
}

void luagame::window_context::set_viewport(glm::ivec4 viewport) {
	this->viewport = viewport;
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}
