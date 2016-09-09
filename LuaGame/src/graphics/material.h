#pragma once

#include "../common.h"

#include <sstream>
#include <fstream>

namespace luagame {
	class material : virtual public reference_counted {
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

		material(const material::options &mtlops);

		virtual ~material();

	public:

		const material::options & opts() { return mtlopts; }

		void bind() const;

		const program_targets & get_targets() const { return targets; }

	private:

		const material::options		mtlopts;

		GLuint						gl_program;
		program_targets				targets;

	};

	std::string get_shader(const GLenum &type, const material::options &options);
	std::string generate_preamble(const material::options &options);
}