#include "texture_object.h"

#include "..\soil\SOIL.h"
#include "..\filesystem.h"


luagame::texture_object::texture_object() {
	glGenTextures(1, &gl_texture);
}

luagame::texture_object::texture_object(const char * filename) {
	std::string true_filename = luagame::filesystem::resolve_resource(filename);

	gl_texture = SOIL_load_OGL_texture(
		true_filename.c_str(),
		SOIL_LOAD_RGBA,
		SOIL_CREATE_NEW_ID,

		SOIL_FLAG_MULTIPLY_ALPHA	| 
		SOIL_FLAG_TEXTURE_REPEATS	| 
		SOIL_FLAG_MIPMAPS
	);

	if (!gl_texture) {
		_log("error loading texture from %s", filename);
		_err("failed to load texture");
	}

	luagame_check_gl_errors();

	_log("loaded texture %d from %s", gl_texture, filename);
}

luagame::texture_object::texture_object(std::string string) : texture_object(string.c_str()) {}

luagame::texture_object::~texture_object() {
	glDeleteTextures(1, &gl_texture);
	_log("deleted texture %d", gl_texture);
}

void luagame::texture_object::bind() {
	glBindTexture(GL_TEXTURE_2D, gl_texture);
}
