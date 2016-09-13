local mesh, transform

local instances

function srnd ( mag )
	return math.random() * mag - mag / 2
end

local axis_of_rotation = luagame.vector(srnd(2), srnd(2), srnd(2)):normalize()

function luagame.load ()
	transform = {
		view		= luagame.matrix(),
		projection	= luagame.matrix()
	}

	transform.view:set_identity()


	mesh = luagame.create_mesh {
		use_texture = true,
		use_transparency = true
	}

	mesh:set_texture("img_test.png")

	local a, b, c, d

	a = {
		pos = {-1, -1, 0},
		tex = {0, 0}
	}

	b = {
		pos = {1, -1, 0},
		tex = {1, 0}
	}

	c = {
		pos = {1, 1, 0},
		tex = {1, 1}
	}

	d = {
		pos = {-1, 1, 0},
		tex = {0, 1}
	}

	mesh:append {
		a, b, c,
		c, d, a
	}

	instances = {}

	for i = 1, 10000 do
		local transform = luagame.matrix()
		
		local dir = luagame.vector(srnd(2), srnd(2), srnd(2)):normalize()
		
		transform:translate(dir * 50 * math.random())

		local angle = srnd(math.pi * 2)
		local axis = luagame.vector(srnd(2), srnd(2), srnd(2)):normalize()

		transform:rotate(angle, axis)
		transform:scale(3)
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