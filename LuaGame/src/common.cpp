#include "common.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

void luagame_check_gl_errors() {
	GLenum err = GL_NO_ERROR;

	while ((err = glGetError()) != GL_NO_ERROR) {
		switch (err) {
		case GL_INVALID_ENUM:
			_err("GL_INVALID_ENUM");
			break;
		case GL_INVALID_VALUE:
			_err("GL_INVALID_VALUE");
			break;
		case GL_INVALID_OPERATION:
			_err("GL_INVALID_OPERATION");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			_err("GL_INVALID_FRAMEBUFFER_OPERATION");
			break;
		case GL_OUT_OF_MEMORY:
			_err("GL_OUT_OF_MEMORY");
			break;
		default:
			_log("encountered unknown gl error value: %d", err);
			_err("unknown gl error");
		}
	}
}

#ifdef _DEBUG

#include <ctime>
#include <cstdio>
#include <cstdarg>

bool _trace(char *format, ...) {
	time_t current_time = time(0); tm now;
	localtime_s(&now, &current_time);

	printf_s("[%02d:%02d:%02d] ", now.tm_hour, now.tm_min, now.tm_sec);

	va_list args;

	va_start(args, format);
	vprintf_s(format, args);
	va_end(args);

	printf("\n");

	return true;
}

#endif