#pragma once

#include "../common.h"
#include "../shared_cache.h"

namespace luagame {

class material_object {
public:
	struct material_options {
		unsigned use_color;
		unsigned use_normal;
		unsigned use_texture;

		unsigned use_depthtest;

		unsigned use_transparency;
		unsigned use_alphamask;

		unsigned use_lighting;
		unsigned int max_lights;

	private:

		auto as_tuple() const {
			return std::tie(
				use_color,
				use_normal,
				use_texture,
				use_depthtest,
				use_transparency,
				use_alphamask,
				use_lighting,
				max_lights
			);
		}

	public:

		bool operator<(const material_object::material_options &rmtl) const {
			const auto &lmtl = *this;

			return lmtl.as_tuple() < rmtl.as_tuple();
		}
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


std::string get_shader(const GLenum &type, const material_object::material_options &options);
std::string generate_preamble(const material_object::material_options &options);

typedef luagame::shared_cache<material_object::material_options, luagame::material_object> material_cache;

}