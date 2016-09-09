lg = luagame

local game = {}

function lg.update (dT)	
	local ground = lg.create_mesh()
	
	ground:add{
		{x=0, 	y=0, 	z=0,	s=0,	t=0}, 
		{x=10, 	y=0, 	z=0,	s=1,	t=1}, 
		{x=10, 	y=10, 	z=0,	s=1,	t=1}, 
		{x=0, 	y=10, 	z=0,	s=0,	t=1}
	}
	
	ground.material = lg.create_material{ use_texture = true }
	ground.texture = lg.create_texture("img_test.png")
	
	local scene = lg.create_scene()
	local camera = lg.create_camera()
	local camera_root = lg.create_empty()
	
	scene:add(ground)
	
	scene:add(camera_root)
	camera_root:add(camera)
end

function lg.draw ()
	
end