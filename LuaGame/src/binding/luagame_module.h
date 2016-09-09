#pragma once

#include "binding.h"

using namespace luagame;

int lg_noop(lua_State * L) { return 0; }

#define LG_PROP_CLOSE					"close_requested"
#define LG_PROP_WINDOW_WIDTH			"window_width"
#define LG_PROP_WINDOW_HEIGHT			"window_height"
#define LG_PROP_WINDOW_TITLE			"window_title"

int lg_index(lua_State * L);
int lg_newindex(lua_State * L);
 
#define LG_QUIT							"quit"

int lg_quit(lua_State * L);

// -- Mesh Type

#define LG_MESH_CREATE					"create_mesh"

int lg_mesh_create(lua_State * L);

int	lg_mesh_index(lua_State * L);
int	lg_mesh_newindex(lua_State * L);

#define LG_MESH_ADD_TRIS				"add_tris"

int	lg_mesh_triangles(lua_State * L);
 
#define LG_MESH_ADD_QUADS				"add_quads"

int	lg_mesh_quads(lua_State * L);

// -- Material Type

#define LG_MATERIAL_CREATE				"create_material"

int lg_material_create(lua_State * L);

#define LG_MATERIAL_PROP_USECOLOR		"use_color"
#define LG_MATERIAL_PROP_USETEXTURE		"use_texture"

int	lg_material_index(lua_State * L);
int	lg_material_newindex(lua_State * L);

// -- Texture Type

#define LG_TEX_CREATE					"create_texture"

int lg_tex_create(lua_State * L);

// -- Scene Type

#define LG_SCENE_CREATE					"create_scene"

int lg_scene_create(lua_State * L);

int	lg_scene_index(lua_State * L);
int	lg_scene_newindex(lua_State * L);

#define LG_SCENE_ADD					"add"

int	lg_scene_add(lua_State * L);
 
#define LG_SCENE_DRAW					"draw"

int	lg_scene_draw(lua_State * L);

// -- Camera Type

#define LG_CAMERA_CREATE				"create_camera"

int lg_camera_create(lua_State * L);

int	lg_camera_index(lua_State * L);
int	lg_camera_newindex(lua_State * L);

// -- Empty Node Type

#define LG_NODE_CREATE					"create_empty"

int lg_node_create(lua_State * L);

int	lg_node_index(lua_State * L);
int	lg_node_newindex(lua_State * L);