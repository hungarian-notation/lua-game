#include "lgapi_gfx.h"
#include "../graphics/material_object.h"

using namespace luagame;

luagame::material_object::material_options luagame_tomaterialoptions(lua_State * L, int idx) {
	material_object::material_options opts = {};
	
	if (lua_istable(L, idx)) {

		lua_getfield(L, idx, "use_color");
		opts.use_color = lua_toboolean(L, -1);

		lua_getfield(L, idx, "use_texture");
		opts.use_texture = lua_toboolean(L, -1);

		lua_getfield(L, idx, "use_normals");
		opts.use_normal = lua_toboolean(L, -1);

		lua_getfield(L, idx, "use_depthtest");
		opts.use_depthtest = lua_toboolean(L, -1);

		lua_getfield(L, idx, "use_transparency");
		opts.use_transparency = lua_toboolean(L, -1);

		lua_getfield(L, idx, "use_alphamask");
		opts.use_alphamask = lua_toboolean(L, -1);

		lua_getfield(L, idx, "use_lighting");
		opts.use_lighting = lua_toboolean(L, -1);

		if (opts.use_lighting) opts.max_lights = 6;

		lua_pop(L, 4);
	}

	return opts;
}

void luagame_pushmetamaterial(lua_State * L) {
	luagame_pushobjmetatable<material_object>(L, nullptr, lgapi_create_material);
}

void luagame_pushmaterial(lua_State * L, std::shared_ptr<luagame::material_object> material) {
	luagame_pushobj<material_object>(L, material, nullptr, lgapi_create_material);
}

int lgapi_create_material(lua_State * L) {
	auto material = luagame_getcontext(L)->material_cache[luagame_tomaterialoptions(L, 1)];
	luagame_pushmaterial(L, material);
	return 1;
}
