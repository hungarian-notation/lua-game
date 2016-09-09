# LuaGame API

## API Conventions

### The Vertex-like Table

A vertex-like table is a table that defines components of a vertex via the presence of certain keys.

<pre>{ 
	x = 0.0, y = 0.0, z = 0.0,
	r = 1.0, g = 0.0, b = 0.0,
	s = 0.0, t = 0.0
}</pre>

#### Position

The position component is defined by the 



## `luagame`

### Hooks

#### `function luagame.initialize ()`

#### `function luagame.update ( dT )`

#### `function luagame.draw ()`

### System

#### `function luagame.quit ()`

### Graphics

#### `function luagame.create_mesh ()`

#### `function luagame.create_material (options)`

#### `function luagame.create_texture (file)`

### Scene Graph

#### `function luagame.create_scene ()`

#### `function luagame.create_camera ()`

#### `function luagame.create_empty ()`



## *object:* `mesh` 

#### `function mesh:add_tris ( vertices )`

Adds one or more triangles to the mesh. Vertices must be an array of a multiple of three vertex-like tables.

#### `function mesh:add_quads ( vertices )`

Adds one or more triangles to the mesh. Vertices must be an array of a multiple of four vertex-like tables.



## *object:* `scene`

#### `function scene:add ( node )`

#### `function scene:draw ()`

## *object:* `scene_node`

