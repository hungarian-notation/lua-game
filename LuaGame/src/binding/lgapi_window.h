#pragma once

#include "binding.h"

namespace luagame {
	namespace api {

		int create_window(lua_State * L);

		namespace window {
			int get_title(lua_State * L);
			int set_title(lua_State * L);

			int get_width(lua_State * L);
			int get_height(lua_State * L);
			int resize(lua_State * L);

			int get_should_close(lua_State * L);
			int set_should_close(lua_State * L);
			int close(lua_State * L);
		}
	}
}