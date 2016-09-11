#pragma once

#include "..\common.h"
#include <lua53\lua.hpp>

#define LUA_METAMETHOD_GC		"__gc"
#define LUA_METAMETHOD_INDEX	"__index"

namespace luagame {
	int release_api_object(lua_State * L);

	void create_api_object(lua_State * L, reference_counted * object);

	template <class T>
	T * to_api_object(lua_State * L, int idx) {
		if (!lua_isnone(L, idx) && lua_isuserdata(L, idx)) {
			T * ptr = dynamic_cast<T *>(*(reference_counted **)lua_touserdata(L, idx));

			if (!ptr) {
				luaL_error(L, "internal error: api object was invalid or of an unexpected type");
				return nullptr; // unreachable due to longjmp
			} else {
				return ptr;
			}
		} else {
			luaL_error(L, "not an api object; did you call with the wrong convention?");
			return nullptr; // unreachable due to longjmp
		}
	}
}