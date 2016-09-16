#pragma once

#include "..\soil\SOIL.h"
#include "..\common.h"

namespace luagame {
	class texture_object {
	public:
		
		texture_object(const char * filename) {
			luagame_check_gl_errors();

			gl_texture = SOIL_load_OGL_texture(
				filename,
				SOIL_LOAD_RGBA,
				SOIL_CREATE_NEW_ID,
				SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_MULTIPLY_ALPHA
			);

			if (!gl_texture) {
				_log("error loading texture from %s", filename);
				_err("failed to load texture");
			}

			luagame_check_gl_errors();

			_log("created texture %d", gl_texture);
		}
		
		texture_object(std::string string) : texture_object(string.c_str()) {}

		~texture_object() {
			glDeleteTextures(1, &gl_texture);
			_log("deleted texture %d", gl_texture);
		}

	public:

		void bind() {
			glBindTexture(GL_TEXTURE_2D, gl_texture); 
		}

	private:

		GLuint gl_texture;
	};
}