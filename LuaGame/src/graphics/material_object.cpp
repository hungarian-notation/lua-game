#include "material_object.h"

#include <sstream>

#include "../res/fragment_glsl.res.h"
#include "../res/vertex_glsl.res.h"

// When defined, this macro causes new materials to dump the source of their shader to console
// immediately before linking.
// #define DUMP_SHADER_TO_CONSOLE 

using namespace std;
using namespace luagame;

namespace {
string load_shader(const string source, const string preamble) {
	stringstream buffer;

	buffer << preamble;
	buffer << endl;
	buffer << source;

	return buffer.str();
}

GLuint create_shader(GLuint mode, const std::string name, const std::string &source) {
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
			_log("in %s: %s\n", name.c_str(), info_log);
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

string luagame::get_shader(const GLenum &type, const material_object::material_options &options) {
	switch (type) {
	case GL_VERTEX_SHADER:
		return load_shader(luagame::resources::vertex_glsl, generate_preamble(options));
	case GL_FRAGMENT_SHADER:
		return load_shader(luagame::resources::fragment_glsl, generate_preamble(options));
	default:
		_log("invalid shader type GLenum: %d", type);
		_err("invalid shader type");
		return string(); // unreachable
	}
}

string luagame::generate_preamble(const material_object::material_options &options) {
	material_object::material_options effective_options(options);

	if (effective_options.use_alphamask) {
		effective_options.use_color = true;
		effective_options.use_texture = true;
		effective_options.use_transparency = true;
	}

	stringstream buf;

	buf << "#version 330" << endl;

	// -- Vertex Components

	if (effective_options.use_color)
		buf << "#define USE_COLOR" << endl;
	if (effective_options.use_normal)
		buf << "#define USE_NORMAL" << endl;
	if (effective_options.use_texture)
		buf << "#define USE_TEXTURE" << endl;

	// -- Behaviors

	if (effective_options.use_transparency)
		buf << "#define USE_TRANSPARENCY" << endl;

	if (effective_options.use_alphamask)
		buf << "#define USE_ALPHAMASK" << endl;

	if (effective_options.use_lighting) {
		buf << "#define USE_LIGHTING" << endl;
		buf << "#define MAX_LIGHTS " << options.max_lights << endl;
	}

	buf << endl;

	return buf.str();
}

luagame::material_object::material_object(const material_object::material_options & mtlopts) : options(mtlopts) {
	std::string vert, frag;

	vert = get_shader(GL_VERTEX_SHADER, mtlopts);
	frag = get_shader(GL_FRAGMENT_SHADER, mtlopts);

	#ifdef DUMP_SHADER_TO_CONSOLE

	_log(
		"\n\n\n"
		"  ------------------------------ VERTEX SHADER ------------------------------  "
		"\n\n\n%s\n\n\n"
		"  ----------------------------- FRAGMENT SHADER -----------------------------  "
		"\n\n\n%s\n\n",

		vert.c_str(), frag.c_str());

	#endif

	gl_program = create_program(vert, frag);

	targets.position_attr = glGetAttribLocation(gl_program, "attr_Position");
	targets.normal_attr = glGetAttribLocation(gl_program, "attr_Normal");
	targets.texcoord_attr = glGetAttribLocation(gl_program, "attr_TexCoord");
	targets.color_attr = glGetAttribLocation(gl_program, "attr_Color");

	targets.model_uni = glGetUniformLocation(gl_program, "u_Model");
	targets.view_uni = glGetUniformLocation(gl_program, "u_View");
	targets.proj_uni = glGetUniformLocation(gl_program, "u_Proj");

	targets.invtrans_uni = glGetUniformLocation(gl_program, "u_InvTrans");

	targets.lightpos_uni = glGetUniformLocation(gl_program, "u_LightPos");
	targets.lightcolor_uni = glGetUniformLocation(gl_program, "u_LightColor");
	targets.ambientlight_uni = glGetUniformLocation(gl_program, "u_AmbientLight");
}

luagame::material_object::~material_object() {
	glDeleteProgram(gl_program);
}

void luagame::material_object::bind() const {
	glUseProgram(gl_program);
}