#pragma once

#include "shared.h"

#include "lgapi_window.h"

namespace luagame {

	namespace api {

		int execute();

		int luaopen_luagame(lua_State * L);

		namespace hooks {

			int load(lua_State * L);

			int update(lua_State * L, float dt);

			int draw(lua_State * L);

		}

		int create_mesh(lua_State * L);

		int create_scene(lua_State * L);

		int create_node(lua_State * L);

	}

}
