#pragma once

#include "../common.h"
#include "../shared_cache.h"

namespace luagame {

class material_object {
public:
	struct material_options {
		unsigned use_color : 1;
		unsigned use_normal : 1;
		unsigned use_texture : 1;
		unsigned use_transparency : 1;

		unsigned use_lighting : 1;
		unsigned int max_lights;
	};

	struct program_targets {
		GLint position_attr,

			normal_attr,
			texcoord_attr,
			color_attr,

			model_uni,
			view_uni,
			invtrans_uni,
			proj_uni,

			lightpos_uni,
			lightcolor_uni,
			ambientlight_uni;
	};
public:

	material_object(const material_object::material_options &mtlops);

	virtual ~material_object();

public:

	const material_object::material_options & opts() const { return options; }

	void bind() const;

	const program_targets & get_targets() const { return targets; }


	const material_object::material_options		options;

	GLuint id() const { return gl_program; }

private:

	GLuint										gl_program;
	program_targets								targets;

};

bool operator <(const material_object::material_options& x, const material_object::material_options& y);

std::string get_shader(const GLenum &type, const material_object::material_options &options);
std::string generate_preamble(const material_object::material_options &options);

typedef luagame::shared_cache<material_object::material_options, luagame::material_object> material_cache;

}