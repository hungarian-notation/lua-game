#pragma once

#include "..\common.h"
#include "..\shared_cache.h"

namespace luagame {
	class texture_object {
	public:
		
		// creates an uninitialized texture
		texture_object();

		texture_object(const char * filename);
		
		texture_object(std::string string);

		~texture_object();

	public:

		void bind();

		GLuint id() const { return gl_texture; }

	private:

		GLuint gl_texture;
	};

	typedef luagame::shared_cache<const char *, luagame::texture_object> texture_cache;
}