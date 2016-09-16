#pragma once

#include "lgapi_core.h"

#include "..\graphics\graphics_math.h"

#include "..\graphics\window_context.h"
#include "..\graphics\texture_object.h"
#include "..\graphics\vertex.h"
#include "..\graphics\material_object.h"
#include "..\graphics\mesh_object.h"

// WINDOW

void luagame_pushwindow(lua_State * L, std::shared_ptr<luagame::window_context> window);

// int luagame_newwindow(lua_State * L);

// TEXTURE

int lgapi_create_texture(lua_State * L);

// MESH

#define LUAGAME_MTLOPT_COLOR		"use_color"
#define LUAGAME_MTLOPT_NORMALS		"use_normals"
#define LUAGAME_MTLOPT_TEXTURE		"use_texture"
#define LUAGAME_MTLOPT_TRANSPARENCY "use_transparency"

int lgapi_create_mesh(lua_State * L);

// VERTEX HANDLING

#define LUAGAME_VERTEX_POSITION		"pos"
#define LUAGAME_VERTEX_COLOR		"clr"
#define LUAGAME_VERTEX_NORMAL		"nml"
#define LUAGAM_VERTEX_TEXCOORD		"tex"

int luagame_isvertex(lua_State * L, int idx);
luagame::vertex luagame_checkvertex(lua_State * L, int idx);
luagame::vertex luagame_tovertex(lua_State * L, int idx);
void luagame_pushvertex(lua_State * L, const luagame::vertex &vert);

int lgapi_vertex_tostring(lua_State * L);

// MATERIAL OPTIONS HANDLING

luagame::material_object::options luagame_tomaterialoptions(lua_State * L, int idx);