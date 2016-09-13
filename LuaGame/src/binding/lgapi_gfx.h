#pragma once

#include "binding.h"

#include "..\graphics\graphics_math.h"

#include "..\graphics\window_context.h"
#include "..\graphics\texture.h"
#include "..\graphics\vertex.h"
#include "..\graphics\material.h"
#include "..\graphics\mesh.h"

#define LUAGAME_WINDOW "luagame_window"

int luagame_newwindow(lua_State * L);

luagame::window_context * luagame_towindow(lua_State * L, int idx);

#define LUAGAME_TEXTURE "luagame_texture"

int lgapi_create_texture(lua_State * L);

int luagame_istexture(lua_State* L, int idx);
luagame::texture * luagame_totexture(lua_State * L, int idx);

// LUAGAME_VERTEX

#define LUAGAME_VERTEX_POSITION		"pos"
#define LUAGAME_VERTEX_COLOR		"clr"
#define LUAGAME_VERTEX_NORMAL		"nml"
#define LUAGAM_VERTEX_TEXCOORD		"tex"

int luagame_isvertex(lua_State * L, int idx);
luagame::vertex luagame_checkvertex(lua_State * L, int idx);
luagame::vertex luagame_tovertex(lua_State * L, int idx);
void luagame_pushvertex(lua_State * L, const luagame::vertex &vert);

int lgapi_vertex_tostring(lua_State * L);

#define LUAGAME_MESH "luagame_mesh"

#define LUAGAME_MTLOPT_COLOR		"use_color"
#define LUAGAME_MTLOPT_NORMALS		"use_normals"
#define LUAGAME_MTLOPT_TEXTURE		"use_texture"
#define LUAGAME_MTLOPT_TRANSPARENCY "use_transparency"

int lgapi_create_mesh(lua_State * L);


luagame::material::options luagame_tomaterialoptions(lua_State * L, int idx);
luagame::mesh_object * luagame_tomesh(lua_State * L, int idx);