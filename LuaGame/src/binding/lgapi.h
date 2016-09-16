#pragma once

#include "lgapi_core.h"
#include "lgapi_math.h"
#include "lgapi_gfx.h"

int luagame_execute();

extern "C" int luaopen_luagame(lua_State * L);
