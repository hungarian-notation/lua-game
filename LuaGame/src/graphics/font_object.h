#pragma once

#include "../common.h"
#include "../filesystem.h"

#include "graphics_math.h"
#include "texture_object.h"

#include <ft2build.h>
#include <map>

#include FT_FREETYPE_H

namespace luagame {

// working from http://learnopengl.com/#!In-Practice/Text-Rendering

class font_object {
public:

	font_object(FT_Library * ft, std::string font_file, FT_UInt size) : size(size) {
		if (FT_New_Face(*ft, luagame::filesystem::resolve_resource(font_file).c_str(), 0, &ft_face)) 
			throw std::runtime_error("failed to load font face");
		if (FT_Set_Pixel_Sizes(ft_face, 0, size)) throw std::runtime_error("error setting font size");
	}

	~font_object() {
		FT_Done_Face(ft_face);
	}

public:

	struct character_data {
		std::shared_ptr<texture_object>	texture;
		glm::ivec2	size;
		glm::ivec2	bearing;
		float		advance;
	};

	character_data get_character(FT_ULong codepoint) {
		if (!characters.count(codepoint)) {
			if (FT_Load_Char(ft_face, codepoint, FT_LOAD_RENDER | FT_LOAD_IGNORE_TRANSFORM))
				return get_character(codepoint != '?' ? '?' : ' ');

			character_data data;

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			data.texture = std::make_shared<texture_object>();
			data.size = glm::ivec2(ft_face->glyph->bitmap.width, ft_face->glyph->bitmap.rows);
			data.bearing = glm::ivec2(ft_face->glyph->bitmap_left, ft_face->glyph->bitmap_top);
			data.advance = ft_face->glyph->advance.x / 64.0F;

			data.texture->bind();

			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				ft_face->glyph->bitmap.width,
				ft_face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				ft_face->glyph->bitmap.buffer
			);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			characters[codepoint] = data;
			return data;
		}

		return characters[codepoint];
	}

	bool get_kerning(FT_UInt left_glyph, FT_UInt right_glyph, glm::vec2 * kerning) {
		FT_Vector ftkern;

		if (FT_Get_Kerning(ft_face, left_glyph, right_glyph, FT_KERNING_UNFITTED, &ftkern)) return false;
		else {
			kerning->x = ftkern.x / 64.0F;
			kerning->y = ftkern.y / 64.0F;
			return true;
		}
	}
	
	FT_UInt get_size() {
		return size;
	}

private:

	FT_Face ft_face;
	const FT_UInt size;
	std::map<FT_ULong, character_data> characters;

};

}