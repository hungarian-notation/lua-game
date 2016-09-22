#include "lgapi.h"
#include "lgapi_gfx.h"
#include "lgmodule.h"

using namespace luagame;
using namespace std;

#define LUAGAME_R_CONTEXT "LUAGAME_CONTEXT"
#define LUAGAME_R_WINDOW "LUAGAME_WINDOW"

namespace {
	void register_context(lua_State * L);
	void register_window(lua_State * L);

	// These functions set fields on the table at the top 
	// of the lua stack that contain either single 
	// functions or sub-modules

	void include_math(lua_State * L);
	void include_graphics(lua_State * L);
}

void lgload_luagame(lua_State * L) {
	register_context(L);
	register_window(L);

	{
		lua_newtable(L); // -- luagame
		lgload_window(L);

		lgload_mouse(L);
		lgload_keyboard(L);

		include_math(L);
		include_graphics(L);
	}

	lua_setfield(L, -2, "luagame");
}

namespace {
	void register_context(lua_State * L) {
		luagame_pushobj<luagame_context>(L, std::make_shared<luagame_context>());
		lua_setfield(L, LUA_REGISTRYINDEX, LUAGAME_R_CONTEXT);
	}

	void register_window(lua_State * L) {
		luagame_pushobj<luagame::window_context>(L, std::make_shared<luagame::window_context>());
		lua_setfield(L, LUA_REGISTRYINDEX, LUAGAME_R_WINDOW);
	}

	void include_math(lua_State * L) {
		luagame_setfunc(L, "matrix", &lgapi_create_matrix);
		luagame_setfunc(L, "vector", &lgapi_create_vector);
	}

	void include_graphics(lua_State * L) {
		luagame_setfunc(L, "create_texture", &lgapi_create_texture);
		luagame_setfunc(L, "create_mesh", &lgapi_create_mesh);
		luagame_setfunc(L, "create_batch", &lgapi_create_batch);
	}
}

std::shared_ptr<luagame_context> luagame_getcontext(lua_State * L) {
	lua_getfield(L, LUA_REGISTRYINDEX, LUAGAME_R_CONTEXT);
	auto context = luagame_checkobj<luagame_context>(L, -1);
	lua_pop(L, 1);
	return context;
}

std::shared_ptr<window_context> luagame_getwindow(lua_State * L) {
	lua_getfield(L, LUA_REGISTRYINDEX, LUAGAME_R_WINDOW);
	auto window = luagame_checkobj<window_context>(L, -1);
	lua_pop(L, 1);
	return window;
}