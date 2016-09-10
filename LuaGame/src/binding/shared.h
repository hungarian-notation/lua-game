#pragma once

#include "..\common.h"
#include <lua53\lua.hpp>

#define LUA_METAMETHOD_GC "__gc"

namespace luagame {

	namespace api {

		int release_api_object(lua_State * L);

		void create_api_object(lua_State * L, reference_counted * object);

	}

}