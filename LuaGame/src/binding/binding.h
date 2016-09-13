#pragma once

#include <lua53\lua.hpp>

#include "..\common.h"

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

void luagame_pushref(lua_State * L, luagame::reference_counted * object);

int lgapi_releaseref(lua_State * L);

template <class reftype> reftype * luagame_unwrapref(void * ref) {
	return dynamic_cast<reftype *>(*(luagame::reference_counted **) ref);
}