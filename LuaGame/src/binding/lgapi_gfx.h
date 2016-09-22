#pragma once

#include "lgapi_core.h"

#include "..\graphics\vertex.h"
#include "..\graphics\material_object.h"
#include "..\graphics\environment_object.h"


int lgapi_create_texture(lua_State * L);

int lgapi_create_mesh(lua_State * L);

int lgapi_create_batch(lua_State * L);

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

luagame::material_object::material_options luagame_tomaterialoptions(lua_State * L, int idx);