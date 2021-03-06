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

	int lgapi_clear(lua_State * L);
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
		std::shared_ptr<luagame_context> context = std::make_shared<luagame_context>();
		luagame_pushobj<luagame_context>(L, context);
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

		luagame_pushmetatexture(L);
		lua_setfield(L, -2, "texture");
		luagame_pushmetamaterial(L);
		lua_setfield(L, -2, "material");
		luagame_pushmetamesh(L);
		lua_setfield(L, -2, "mesh");

		luagame_pushmetabatch(L);
		lua_setfield(L, -2, "batch");
		luagame_pushmetafont(L);
		lua_setfield(L, -2, "font");

		luagame_setfunc(L, "clear",				&lgapi_clear);
	}

	static const char * clear_targets[] = {
		"color_buffer", "depth_buffer",
		NULL
	};

	int lgapi_clear(lua_State * L) {
		int args = lua_gettop(L);

		for (int i = 1; i <= args; ++i) {
			int opt = luaL_checkoption(L, i, NULL, clear_targets);
			switch (opt) {
			case 0: 
				glClear(GL_COLOR_BUFFER_BIT);
				break;
			case 1:
				glClear(GL_DEPTH_BUFFER_BIT);
				break;
			default:
				luaL_argerror(L, i, "unhandled clear target");
			}
		}

		return 0;
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