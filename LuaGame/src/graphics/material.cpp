#include "material.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;
using namespace luagame;

namespace {
	string load_shader(const string filename, const string preamble) {
		stringstream buffer;

		buffer << preamble;
		buffer << ifstream(filename).rdbuf();

		return buffer.str();
	}

	GLuint create_shader(GLuint mode, const std::string file, const std::string &source) {
		GLuint shader_id = glCreateShader(mode);

		const char * const c_str_source = source.c_str();

		glShaderSource(shader_id, 1, &c_str_source, NULL);
		glCompileShader(shader_id);

		int log_length;

		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

		if (log_length > 0) {
			char *info_log = new char[log_length];
			int chars_written;
			glGetShaderInfoLog(shader_id, log_length, &chars_written, info_log);
			if (chars_written > 0)
				_log("in %s: %s\n", file.c_str(), info_log);
			delete[] info_log;
		}

		return shader_id;
	}

	GLuint create_program(const std::string &vert_source, const std::string &frag_source) {
		GLuint vert_shader = create_shader(GL_VERTEX_SHADER, "vertex shader", vert_source);
		GLuint frag_shader = create_shader(GL_FRAGMENT_SHADER, "fragment shader", frag_source);

		GLuint program = glCreateProgram();

		glAttachShader(program, vert_shader);
		glAttachShader(program, frag_shader);

		glLinkProgram(program);

		int log_length; glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

		if (log_length > 0) {
			char *info_log = new char[log_length];
			int written;
			glGetProgramInfoLog(program, log_length, &written, info_log);
			if (written > 0)
				_log("shader link error: %s\n", info_log);
			delete[] info_log;
		}

		glDetachShader(program, vert_shader);
		glDetachShader(program, frag_shader);

		glDeleteShader(vert_shader);
		glDeleteShader(frag_shader);

		return program;
	}
}

string luagame::get_shader(const GLenum &type, const material::options &options) {
	switch (type) {
	case GL_VERTEX_SHADER:
		return load_shader("vertex.glsl", generate_preamble(options));
	case GL_FRAGMENT_SHADER:
		return load_shader("fragment.glsl", generate_preamble(options));
	default:
		_log("invalid shader type GLenum: %d", type);
		_err("invalid shader type");
	}
}

string luagame::generate_preamble(const material::options &options) {
	stringstream buf;

	buf << "#version 330" << endl;

	// -- Vertex Components

	if (options.use_position)
		buf << "#define USE_POSITION" << endl;
	if (options.use_color)
		buf << "#define USE_COLOR" << endl;
	if (options.use_normal)
		buf << "#define USE_NORMAL" << endl;
	if (options.use_texture)
		buf << "#define USE_TEXTURE" << endl;

	// -- Behaviors

	if (options.use_transparency)
		buf << "#define USE_TRANSPARENCY" << endl;

	buf << endl;

	return buf.str();
}

luagame::material::material(const material::options & mtlopts) : mtlopts(mtlopts) {
	std::string vert, frag;

	vert = get_shader(GL_VERTEX_SHADER, mtlopts);
	frag = get_shader(GL_FRAGMENT_SHADER, mtlopts);

	_log(
		"\n\n\n"
		"  ------------------------------ VERTEX SHADER ------------------------------  "
		"\n\n\n%s\n\n\n"
		"  ----------------------------- FRAGMENT SHADER -----------------------------  "
		"\n\n\n%s\n\n",

		vert.c_str(), frag.c_str());

	gl_program = create_program(vert, frag);

	targets.position_attr = glGetAttribLocation(gl_program, "attr_Position");
	targets.normal_attr = glGetAttribLocation(gl_program, "attr_Normal");
	targets.texcoord_attr = glGetAttribLocation(gl_program, "attr_TexCoord");
	targets.color_attr = glGetAttribLocation(gl_program, "attr_Color");

	targets.model_uni = glGetUniformLocation(gl_program, "u_Model");
	targets.view_uni = glGetUniformLocation(gl_program, "u_View");
	targets.proj_uni = glGetUniformLocation(gl_program, "u_Proj");
}

luagame::material::~material() {
	glDeleteProgram(gl_program);
}

void luagame::material::bind() const {
	glUseProgram(gl_program);
}