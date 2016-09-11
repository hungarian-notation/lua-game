#pragma once

#include "lgapi_shared.h"

namespace luagame {
	namespace api {

		int create_window(lua_State * L);

		int window_get_title(lua_State * L);
		int window_set_title(lua_State * L);

		int window_get_width(lua_State * L);
		int window_get_height(lua_State * L);
		int window_resize(lua_State * L);

		int window_get_should_close(lua_State * L);
		int window_set_should_close(lua_State * L);
		int window_close(lua_State * L);

	}
}