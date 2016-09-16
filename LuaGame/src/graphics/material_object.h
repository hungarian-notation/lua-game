#pragma once

#include "../common.h"
#include "../gl_headers.h"

#include <sstream>
#include <fstream>
#include <tuple>

namespace luagame {
	class material_object {
	public:

		struct options {
			unsigned use_position : 1;
			unsigned use_color : 1;
			unsigned use_normal : 1;
			unsigned use_texture : 1;
			unsigned use_transparency : 1;

		};


		struct program_targets {
			GLint position_attr,
				normal_attr,
				texcoord_attr,
				color_attr,

				model_uni,
				view_uni,
				proj_uni;
		};

	public:

		material_object(const material_object::options &mtlops);

		virtual ~material_object();

	public:

		const material_object::options & opts() { return mtlopts; }

		void bind() const;

		const program_targets & get_targets() const { return targets; }

	private:

		const material_object::options		mtlopts;

		GLuint						gl_program;
		program_targets				targets;

	};

	bool operator <(const material_object::options& x, const material_object::options& y);

	std::string get_shader(const GLenum &type, const material_object::options &options);
	std::string generate_preamble(const material_object::options &options);
}