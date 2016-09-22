local skybox = require "skybox"

local mesh_batch

local scene = {}

local skybox_meshes

local player = {
	position = luagame.vector(0, 0, 0),
	height = 1.8,
	angle = 0
}

local environment = {
	ambient_light = { 0.2, 0.2, 0.2 },

	lights = {
		{ direction = { 1, 1, 1 }, color = { 0.6, 0.6, 0.6 } }
	}
}

local keyboard_state = {}

function luagame.key_pressed (key, scancode, mods) keyboard_state[key] = true end
function luagame.key_released (key, scancode, mods) keyboard_state[key] = false end

function gauss (x, a, b, c) 
	return a * math.exp(-((x-b) * (x-b)) / (2 * c * c));
end

function gauss_hill (xpos, ypos, height, radius)
	return function (x, y)
		return gauss(x - xpos, 1, 1, radius) * gauss(y - ypos, 1, 1, radius) * height
	end
end

function dunes (opts)

	local xscale = opts.xscale or 30
	local xoff = opts.xoff or 0
	local yscale = opts.yscale or 30
	local yoff = opts.yoff or 0
	local height = opts.height or 1

	return function (x, y)
		return math.sin(math.pi * (x - xoff) / xscale) * math.sin(math.pi * (y - yoff) / yscale) * height
	end
end

local functors = { 
	gauss_hill (0, 0, 40, 100),
	gauss_hill (100, -150, 10, 30),
	dunes { yoff = 15, yscale = 20, height = 0.5 },
	dunes { xscale = 100, yscale = 100, xoff = 20, yoff = 15, height = 5 }
};

function sample_height (x, y)
	local height = 0

	for i, functor in pairs(functors) do
		height = height + functor(x, y)
	end

	return height
end

local grid_size = 100

local grid = { xmin = -grid_size, xmax = grid_size, ymin = -grid_size, ymax = grid_size, cell_size = 4, tex_scale = 0.1 }

function sample_normal (x, y)
	local origin = luagame.vector(x, y, sample_height(x, y))

	local a = luagame.vector(x + grid.cell_size, y, sample_height(x + grid.cell_size, y))
	local b = luagame.vector(x, y + grid.cell_size, sample_height(x, y + grid.cell_size))

	local a_off = a - origin;
	local b_off = b - origin;
	
	local cross = a_off:cross(b_off):normalize();

	return cross
end

function build_ground ()
	local ground_mesh = luagame.create_mesh {
		use_texture = true,
		use_normals = true,
		use_lighting = true
	}

	ground_mesh:set_texture("grass.jpg")

	vertices = {}

	for gx = grid.xmin, grid.xmax do
		for gy = grid.xmin, grid.xmax do
			local a, b, c, d

			local xmin, ymin = gx * grid.cell_size,	gy * grid.cell_size
			local xmax, ymax = xmin + grid.cell_size, ymin + grid.cell_size

			a = { vertex = {xmin, ymin, sample_height(xmin, ymin)},	texcoord = {xmin * grid.tex_scale, ymin * grid.tex_scale}, normal = sample_normal(xmin, ymin) }
			b = { vertex = {xmax, ymin, sample_height(xmax, ymin)},	texcoord = {xmax * grid.tex_scale, ymin * grid.tex_scale}, normal = sample_normal(xmax, ymin) }
			c = { vertex = {xmax, ymax, sample_height(xmax, ymax)},	texcoord = {xmax * grid.tex_scale, ymax * grid.tex_scale}, normal = sample_normal(xmax, ymax) }
			d = { vertex = {xmin, ymax, sample_height(xmin, ymax)},	texcoord = {xmin * grid.tex_scale, ymax * grid.tex_scale}, normal = sample_normal(xmin, ymax) }
			
			vertices[#vertices + 1] = a
			vertices[#vertices + 1] = b
			vertices[#vertices + 1] = c
			vertices[#vertices + 1] = c
			vertices[#vertices + 1] = d
			vertices[#vertices + 1] = a
		end
	end
	
	print(#vertices)

	ground_mesh:append(vertices)

	return ground_mesh;
end

function luagame.load ()
	transform = {
		view		= luagame.matrix(),
		projection	= luagame.matrix()
	}

	transform.view:set_identity()

	scene[#scene + 1] = { mesh=build_ground(), matrix=luagame.matrix() }

	skybox_meshes = skybox.create(
		"bluecloud_ft.jpg",
		"bluecloud_bk.jpg",
		"bluecloud_lf.jpg",
		"bluecloud_rt.jpg",
		"bluecloud_up.jpg",
		"bluecloud_dn.jpg"
	);
end

function update_player (dt) 

	local rot = 0
	local rotspeed = 2

	if keyboard_state[luagame.keyboard.keys.left] then
		rot = rot + dt * rotspeed
	end

	if keyboard_state[luagame.keyboard.keys.right] then
		rot = rot - dt * rotspeed
	end

	player.angle = rot + player.angle

	local translate = luagame.vector()

	local speed = 4

	local motion = speed * dt

	if (keyboard_state[luagame.keyboard.keys.a]) then
		translate.x = translate.x - motion
	end
	
	if (keyboard_state[luagame.keyboard.keys.d]) then
		translate.x = translate.x + motion
	end

	if (keyboard_state[luagame.keyboard.keys.w]) then
		translate.y = translate.y + motion
	end

	if (keyboard_state[luagame.keyboard.keys.s]) then
		translate.y = translate.y - motion
	end
	
	local move_transform = luagame.matrix()

	move_transform:rotate(player.angle, {0, 0, 1})
	translate = move_transform * translate

	player.position = player.position + translate
	player.position.z = sample_height(player.position.x, player.position.y)
end

function luagame.update (dt)
	update_player(dt)

	transform.view:set_identity()

	transform.view:translate(player.position.x, player.position.y, player.position.z + player.height)
	transform.view:rotate(math.pi / 2, {1, 0, 0})
	transform.view:rotate(player.angle, {0, 1, 0})
	
	local width, height = luagame.window:get_width(), luagame.window:get_height()
	transform.projection:set_perspective(math.pi / 3, width, height, 1.0, 450.0)
end

function luagame.draw () 
	mesh_batch = mesh_batch or luagame.create_batch()
	
	mesh_batch:begin(transform.view:get_inverse(), transform.projection, environment)

	for i, model in pairs(scene) do
		mesh_batch:add(model.mesh, model.matrix);
	end

	local skybox_transform = luagame.matrix()

	skybox_transform:set(4, 1, player.position.x)
	skybox_transform:set(4, 2, player.position.y)
	skybox_transform:set(4, 3, player.position.z + player.height)

	for i, mesh in pairs(skybox_meshes) do
		mesh_batch:add(mesh, skybox_transform);
	end

	mesh_batch:flush()
end