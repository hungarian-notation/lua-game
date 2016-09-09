#pragma once

#include <lua53\lua.hpp>

#include "..\common.h"
#include "..\graphics\graphics_context.h"
#include "binding_conf.h"

#define lg_context(L) get_luagame_state(L)->context
#define lg_state(L) get_luagame_state(L)

namespace luagame {
	struct luagame_state {
		graphics_context *	context;
		bool				quit_invoked;
	};

	luagame_state * get_luagame_state(lua_State * L);

	void print_error(lua_State* L, const char * context = "error in client code");

	int luaopen_luagame(lua_State * L);
	
	bool initialize(lua_State* L);

	int binding_execute();

	void binding_initialize(lua_State * L);

	void binding_update(lua_State * L, float dT);

	void binding_draw(lua_State * L);


}