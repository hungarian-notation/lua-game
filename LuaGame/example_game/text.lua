local text = {} ; text.__index = text

function text.create (str, font, scale)
	local cursor = 0

	scale = scale or 1

	local object = setmetatable({}, text)

	object.quads = {}

	for i = 1, #str do
		local chardata = font:get_character(str:sub(i, i))
		local quad = luagame.mesh { 
			use_texture = true, 
			use_alphamask = true,
			use_color = true 
		}

		object.quads[#object.quads + 1] = quad

		quad:set_texture(chardata.texture)

		local a, b, c, d
		local cx, cy = cursor + chardata.bearing.x * scale, chardata.bearing.y * scale
		local hw = chardata.size.x * scale
		local hh = chardata.size.y * scale
		
		a = { vertex = luagame.vector{ cx,		cy - hh,	0 },	texcoord = { 0, 1 }, color = { 1, 1, 1 }  }
		b = { vertex = luagame.vector{ cx + hw,	cy - hh,	0 },	texcoord = { 1, 1 }, color = { 1, 1, 1 }  }
		c = { vertex = luagame.vector{ cx + hw,	cy,			0 },	texcoord = { 1, 0 }, color = { 1, 1, 1 }  }
		d = { vertex = luagame.vector{ cx,		cy,			0 },	texcoord = { 0, 0 }, color = { 1, 1, 1 }  }

		quad:append {a, b, c, c, d, a}

		local advance = chardata.advance

		if i < #str then
			local kerning = font:get_kerning(str:sub(i, i), str:sub(i+1, i+1));
			if kerning then
				advance = advance + kerning.x
			end
		end

		cursor = cursor + advance * scale
	end

	return object
end

return text