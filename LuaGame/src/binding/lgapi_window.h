#pragma once

#include "shared.h"

namespace luagame {

	namespace api {

		int create_window(lua_State * L);

		int window_set_title(lua_State * L);

		int window_resize(lua_State * L);

		int window_close(lua_State * L);

		int window_width(lua_State * L);

		int window_height(lua_State * L);

		int window_resize(lua_State * L);

		int window_title(lua_State * L);

		int window_set_title(lua_State * L);

	}

}