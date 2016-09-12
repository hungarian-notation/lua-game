#pragma once

#include "..\common.h"
#include <lua53\lua.hpp>

#define LUA_METAMETHOD_CALL		"__call"
#define LUA_METAMETHOD_GC		"__gc"
#define LUA_METAMETHOD_INDEX	"__index"
#define LUA_METAMETHOD_NEWINDEX	"__newindex"
#define LUA_METAMETHOD_LEN		"__len"
#define LUA_METAMETHOD_TOSTRING	"__tostring"

#define LUA_METAMETHOD_ADD	"__add"
#define LUA_METAMETHOD_SUB	"__sub"
#define LUA_METAMETHOD_MUL	"__mul"
#define LUA_METAMETHOD_DIV	"__div"

#define LUA_METAMETHOD_UNM	"__unm"
#define LUA_METAMETHOD_NOT	"__bnot"

#define LUA_METAMETHOD_EQ	"__eq"

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

	static int illegal_operation(lua_State * L) {
		return luaL_error(L, "illegal operation");
	}
}