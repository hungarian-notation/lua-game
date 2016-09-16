#pragma once
#include "lgapi_core.h"

void lgload_luagame(lua_State * L);

void lgload_window(lua_State * L);

void lgload_keyboard(lua_State * L);
void lgload_mouse(lua_State * L);

void lgload_graphics(lua_State * L);
void lgload_sound(lua_State * L);