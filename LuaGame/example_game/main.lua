local skybox = require "skybox"

local mesh_batch

local scene = {}

local axis_of_rotation = luagame.vector(0, 0, 1):normalize()

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

function build_ground ()
	local ground_mesh = luagame.create_mesh {
		use_texture = true,
		use_normals = true,
		use_lighting = true
	}

	ground_mesh:set_texture("grass.jpg")

	local a, b, c, d
	local min, max = -100, 100

	a = { vertex = {min, min, 0},	texcoord = {0, 0},	normal = {0, 0, 1} }
	b = { vertex = {max, min, 0},	texcoord = {30, 0},	normal = {0, 0, 1} }
	c = { vertex = {max, max, 0},	texcoord = {30, 30},	normal = {0, 0, 1} }
	d = { vertex = {min, max, 0},	texcoord = {0, 30},	normal = {0, 0, 1} }

	ground_mesh:append {
		a, b, c,
		c, d, a
	}

	return ground_mesh;
end

function luagame.load ()
	transform = {
		view		= luagame.matrix(),
		projection	= luagame.matrix()
	}

	transform.view:set_identity()

	scene[#scene] = { mesh=build_ground(), matrix=luagame.matrix() }
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
end

function luagame.update (dt)
	update_player(dt)

	transform.view:set_identity()

	transform.view:translate(player.position.x, player.position.y, player.position.z + player.height)
	transform.view:rotate(math.pi / 2, {1, 0, 0})
	transform.view:rotate(player.angle, {0, 1, 0})

	local x = transform.view:get(4, 1);
	local y = transform.view:get(4, 2);
	local z = transform.view:get(4, 3);

	luagame.window.set_title(tostring(luagame.vector(x, y, z)))

	local width, height = luagame.window:get_width(), luagame.window:get_height()
	transform.projection:set_perspective(math.pi / 3, width, height, 1.0, 300.0)
end

function luagame.draw () 
	mesh_batch = mesh_batch or luagame.create_batch()
	
	mesh_batch:begin(transform.view:get_inverse(), transform.projection, environment)

	for i, model in pairs(scene) do
		mesh_batch:add(model.mesh, model.matrix);
	end

	mesh_batch:flush()
end