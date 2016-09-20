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


struct luagame_context {
	luagame::shared_cache<const char *, luagame::texture_object>						texture_cache;
	luagame::shared_cache<luagame::material_object::options, luagame::material_object>	material_cache;
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
}

template <typename T>
std::shared_ptr<T> luagame_checkobj(lua_State * L, int idx, const char * tname = typeid(T).name()) {
	return _luagame_header_local::_checkshared<T>(L, idx, tname);
}

template <typename T>
void luagame_pushobj(lua_State * L, std::shared_ptr<T> ptr, luaL_Reg * metatable = NULL, bool self_indexed = true, const char * tname = typeid(T).name()) {
	_luagame_header_local::_pushshared<T>(L, ptr);

	if (luaL_newmetatable(L, tname)) {

		if (metatable)
			luaL_setfuncs(L, metatable, 0);

		lua_pushcfunction(L, &_luagame_header_local::_releaseshared<T>);
		lua_setfield(L, -2, LUA_METAMETHOD_GC);

		if (self_indexed) {
			lua_pushvalue(L, -1);
			lua_setfield(L, -1, LUA_METAMETHOD_INDEX);
		}
	}

	lua_setmetatable(L, -2);
}

template <typename T>
bool luagame_isobj(lua_State * L, int idx, const char * tname = typeid(T).name()) {
	return luaL_testudata(L, idx, tname) != NULL;
}