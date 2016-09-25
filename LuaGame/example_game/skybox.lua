local skybox = {} ; skybox.__index = skybox

local vec = luagame.vector

local function push_plane(models, texture, origin, exa, exb)
	local a, b, c, d

	local vertices = {}

	exa = exa * 1.005
	exb = exb * 1.005

	a = {
		vertex = origin + vec(exa.x, exa.y, exa.z),
		texcoord = {1, 0}
	}
	
	b = {
		vertex = origin + vec(exa.x, exa.y, exb.z),
		texcoord = {1, 1}
	}

	c = {
		vertex = origin + vec(exb.x, exb.y, exb.z),
		texcoord = {0, 1}
	}

	d = {
		vertex = origin + vec(exb.x, exb.y, exa.z),
		texcoord = {0, 0}
	}

	vertices[#vertices + 1] = a
	vertices[#vertices + 1] = b
	vertices[#vertices + 1] = c
	vertices[#vertices + 1] = c
	vertices[#vertices + 1] = d
	vertices[#vertices + 1] = a

	local mesh = luagame.mesh { use_texture = true, use_depthtest = true }
	mesh:set_texture(texture)
	mesh:append(vertices)

	models[#models + 1] = mesh
end

function skybox.create(px_tex, nx_tex, py_tex, ny_tex, pz_tex, nz_tex, size) 
	size = size or 100

	local models = {}

	push_plane(models, px_tex, vec(size, 0, 0),	vec(0,  size, size) ,	vec(0, -size, -size) ) -- px
	push_plane(models, nx_tex, vec(-size, 0, 0),	vec(0, -size, size) ,	vec(0,  size, -size) ) -- nx
	push_plane(models, py_tex, vec(0, size, 0),	vec(-size, 0, size) ,	vec( size, 0, -size) ) -- py
	push_plane(models, ny_tex, vec(0, -size, 0),	vec( size, 0, size) ,	vec(-size, 0, -size) ) -- ny

	return models;
end

return skybox;