-- 'luagame' is automatically in the global table
-- We hook into it by writing specially named functions.

lg = luagame

local game = {}

function lg.update (dT)
	game.scene = lg.newScene()
	game.camera = game.scene.addCamera()
end

function lg.draw (dT)

end