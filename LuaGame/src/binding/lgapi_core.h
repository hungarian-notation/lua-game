#pragma once

#include <lua53\lua.hpp>
#include <typeinfo>

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

#include "../shared_cache.h"
#include "../graphics/texture_object.h"
#include "../graphics/material_object.h"
#include "../window_context.h"

#include <ft2build.h>
#include FT_FREETYPE_H

struct luagame_context {
	luagame::texture_cache	texture_cache;
	luagame::material_cache	material_cache;
	FT_Library freetype;
};

std::shared_ptr<luagame_context> luagame_getcontext(lua_State * L);
std::shared_ptr<luagame::window_context> luagame_getwindow(lua_State * L);

// t.name = func -- where t is the table at idx
void luagame_setfunc(lua_State * L, int idx, const char * name, lua_CFunction func);

// t.name = func -- where t is the table at the top of the stack
void luagame_setfunc(lua_State * L, const char * name, lua_CFunction func);

int luagame_callhook(lua_State * L, const char * hook, int nargs = 0, int nres = 0);

namespace _luagame_header_local {
	template <typename T>
	void _pushshared(lua_State * L, std::shared_ptr<T> ptr) {
		std::shared_ptr<T> ** udata = (std::shared_ptr<T> **) lua_newuserdata(L, sizeof(std::shared_ptr<T>*));
		(*udata) = new std::shared_ptr<T>(ptr);

		_log("lua: created api object @%p", (*udata)->get());
	}

	template <typename T>
	int _releaseshared(lua_State*L) {
		std::shared_ptr<T> ** udata = (std::shared_ptr<T> **) lua_touserdata(L, 1);

		_log("lua: destroying api object @%p", (*udata)->get());

		delete *udata;
		return 0;
	}

	template <typename T>
	std::shared_ptr<T> _checkshared(lua_State * L, int idx, const char * tname) {
		return ** (std::shared_ptr<T> **) luaL_checkudata(L, idx, tname);
	}

	static int lg__callproxy(lua_State * L) {
		int args = lua_gettop(L) - 1; // L[1] is the table that was called. The rest are args.
		
		lua_rotate(L, 1, -1); // rotate L[1] (called table) to L[-1]
		lua_pop(L, 1); // remove L[-1] (called table) from stack

		lua_pushvalue(L, lua_upvalueindex(1)); // push the proxied lua_CFunction onto the top of the stack.
		lua_rotate(L, 1, 1); // rotate the proxied function from L[-1] to L[1]

		lua_call(L, args, LUA_MULTRET);
		return lua_gettop(L);
	}
}

static void luagame_pushcallproxy(lua_State * L, lua_CFunction function) {
	lua_pushcfunction(L, function);
	lua_pushcclosure(L, _luagame_header_local::lg__callproxy, 1);
}

template <typename T>
std::shared_ptr<T> luagame_checkobj(lua_State * L, int idx, const char * tname = typeid(T).name()) {
	return _luagame_header_local::_checkshared<T>(L, idx, tname);
}


template <typename T>
void luagame_pushobjmetatable(lua_State * L, luaL_Reg * functions, lua_CFunction constructor = nullptr, bool self_indexed = true, const char * tname = typeid(T).name()) {
	if (luaL_newmetatable(L, tname)) {
		if (functions)
			luaL_setfuncs(L, functions, 0);

		lua_pushcfunction(L, &_luagame_header_local::_releaseshared<T>);
		lua_setfield(L, -2, LUA_METAMETHOD_GC);

		if (self_indexed) {
			lua_pushvalue(L, -1);
			lua_setfield(L, -1, LUA_METAMETHOD_INDEX);
		}

		if (constructor) {
			lua_createtable(L, 0, 1);
			luagame_pushcallproxy(L, constructor);
			lua_setfield(L, -2, LUA_METAMETHOD_CALL);
			lua_setmetatable(L, -2);
		}
	}
}

template <typename T>
void luagame_pushobj(lua_State * L, std::shared_ptr<T> ptr, luaL_Reg * metatable = NULL, lua_CFunction constructor = nullptr, bool self_indexed = true, const char * tname = typeid(T).name()) {
	_luagame_header_local::_pushshared<T>(L, ptr);
	luagame_pushobjmetatable<T>(L, metatable, constructor, self_indexed, tname);
	lua_setmetatable(L, -2);
}

template <typename T>
bool luagame_isobj(lua_State * L, int idx, const char * tname = typeid(T).name()) {
	return luaL_testudata(L, idx, tname) != NULL;
}