local mesh, transform

local instances

function srnd ( mag )
	return math.random() * mag - mag / 2
end

local axis_of_rotation = luagame.vector(1, 0, 0):normalize()

luagame.window.set_title("This is the new title!")

function luagame.load ()
	transform = {
		view		= luagame.matrix(),
		projection	= luagame.matrix()
	}

	transform.view:set_identity()

	mesh = luagame.create_mesh {
		use_texcoord = true, 
		use_normal = true,

		transparency = true
	}

	mesh:set_texture("img_test.png")

	local a, b, c, d

	a = { vertex = {-1, -1, 0},	texcoord = {0, 0}, normal = {0, 0, 1} }
	b = { vertex = {1, -1, 0},	texcoord = {1, 0}, normal = {0, 0, 1} }
	c = { vertex = {1, 1, 0},	texcoord = {1, 1}, normal = {0, 0, 1} }
	d = { vertex = {-1, 1, 0},	texcoord = {0, 1}, normal = {0, 0, 1} }

	local function reverse(vertex) 
		return {vertex = vertex.vertex, color = vertex.color, texcoord = vertex.texcoord, normal = luagame.vector(vertex.normal) * -1};
	end

	mesh:append {
		a, b, c,
		c, d, a,

		reverse(a), reverse(d), reverse(c),
		reverse(c), reverse(b), reverse(a)
	}

	instances = {}

	for i = 1, 300 do
		local transform = luagame.matrix()
		
		local dir = luagame.vector(srnd(2), srnd(2), srnd(2)):normalize()
		
		transform:translate(dir * 50 * math.random())

		local angle = srnd(math.pi * 2)
		local axis = luagame.vector(srnd(2), srnd(2), srnd(2)):normalize()

		transform:rotate(angle, axis)
		transform:scale(5)
		instances[i] = transform 
	end
end

function luagame.update (dt)
	transform.view:rotate((1 / 2) * math.pi * dt, axis_of_rotation)
	local width, height = luagame.window:get_width(), luagame.window:get_height()
	transform.projection:set_perspective(math.pi / 2, width, height, 1.0, 300.0)
end

function luagame.draw ()
	local view = luagame.matrix():translate(0, 0, -100) * transform.view
	for i, model_matrix in pairs(instances) do
		mesh:draw(model_matrix, view, transform.projection)
	end
end