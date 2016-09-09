-- 'luagame' is automatically in the global table
-- We hook into it by writing specially named functions.

lg = luagame

local game = {}

function lg.update (dT)
	game.camera = lg.camera()
	game.wall = lg.mesh()
	
	local wall = game.wall
	
	wall.add_quad(
		{x=0, 	y=0, 	z=0,	s=0,	t=0}, 
		{x=10, 	y=0, 	z=0,	s=1,	t=1}, 
		{x=10, 	y=10, 	z=0,	s=1,	t=1}, 
		{x=0, 	y=10, 	z=0,	s=0,	t=1}
	)
	
	wall.material = lg.material{
		use_texture = true
	}
	
	wall.texture = lg.texture("img_test.png")
	
	lg.mesh()
end

function lg.draw ()
	
end