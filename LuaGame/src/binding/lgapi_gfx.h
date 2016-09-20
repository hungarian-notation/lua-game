#pragma once

#include "lgapi_core.h"

#include "..\graphics\graphics_math.h"

#include "..\window_context.h"
#include "..\graphics\texture_object.h"
#include "..\graphics\vertex.h"
#include "..\graphics\material_object.h"
#include "..\graphics\mesh_object.h"

// TEXTURE

int lgapi_create_texture(lua_State * L);

// MESH

#define LUAGAME_MTLOPT_COLOR		"use_color"
#define LUAGAME_MTLOPT_NORMALS		"use_normal"
#define LUAGAME_MTLOPT_TEXTURE		"use_texcoord"
#define LUAGAME_MTLOPT_TRANSPARENCY "transparency"

int lgapi_create_mesh(lua_State * L);

// VERTEX HANDLING

#define LUAGAME_VERTEX_POSITION		"vertex"
#define LUAGAME_VERTEX_COLOR		"color"
#define LUAGAME_VERTEX_NORMAL		"normal"
#define LUAGAM_VERTEX_TEXCOORD		"texcoord"

int luagame_isvertex(lua_State * L, int idx);
luagame::vertex luagame_checkvertex(lua_State * L, int idx);
luagame::vertex luagame_tovertex(lua_State * L, int idx);
void luagame_pushvertex(lua_State * L, const luagame::vertex &vert);

int lgapi_vertex_tostring(lua_State * L);

// MATERIAL OPTIONS HANDLING

luagame::material_object::options luagame_tomaterialoptions(lua_State * L, int idx);