#include "lgapi_gfx.h"
#include "../graphics/mesh_batch.h"
#include "lgapi_math.h"

using namespace luagame;
using namespace glm; 

namespace {

environment_object luagame_toenvironment(lua_State * L, int idx);

int mesh_batch_begin(lua_State * L);

int mesh_batch_add(lua_State * L);

int mesh_batch_flush(lua_State * L);

}

int lgapi_create_batch(lua_State * L) {

	luaL_Reg functions[] = {
		{ "begin",	mesh_batch_begin },
		{ "add",	mesh_batch_add },
		{ "flush",	mesh_batch_flush },

		{ NULL, NULL }
	};

	std::shared_ptr<mesh_batch> batch = std::make_shared<mesh_batch>();

	luagame_pushobj<mesh_batch>(L, batch, functions);

	return 1;
}

namespace {

light luagame_tolight(lua_State * L, int idx) {
	if (lua_type(L, idx) != LUA_TTABLE) {
		luaL_error(L, "bad type for light table: fount %s", luaL_typename(L, idx));
	}

	lua_getfield(L, idx, "color");
	if (lua_isnil(L, -1)) luaL_error(L, "light table missing 'color'");

	vec3 color = luagame_tovec3(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, idx, "position");

	if (!lua_isnil(L, -1)) {
		vec3 position = luagame_tovec3(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, idx, "radius");
		if (lua_isnil(L, -1)) luaL_error(L, "point light table missing 'radius'");
		float radius = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);

		return light(position, radius, color);
	} else {
		lua_pop(L, 1); // "position"

		lua_getfield(L, idx, "direction");
		if (lua_isnil(L, -1)) luaL_error(L, "light table requires either 'position' or 'direction'");

		vec3 direction = luagame_tovec3(L, -1);
		lua_pop(L, 1);

		return light(direction, color);
	}
}

environment_object luagame_toenvironment(lua_State * L, int idx) {
	if (!lua_istable(L, idx)) {
		luaL_error(L, "expected an environment table");
		return environment_object();
	} else {
		vec3 ambient_light(0.1F);

		lua_getfield(L, idx, "ambient_light");
		if (!lua_isnil(L, -1))
			ambient_light = luagame_tovec3(L, -1);
		lua_pop(L, 1);

		environment_object env(ambient_light);

		if (lua_getfield(L, idx, "lights") != LUA_TTABLE)
			luaL_error(L, "'lights' key is of unexpected type: %s", luaL_typename(L, -1));
		
		lua_len(L, -1);
		int light_count = (int)lua_tointeger(L, -1);
		lua_pop(L, 1);

		for (int i = 1; i <= light_count; i++) {
			if (lua_geti(L, -1, i)!= LUA_TTABLE) 
				luaL_error(L, "element #%d of light table was an unexpected type: %s", i, luaL_typename(L, -1));

			env.add_light(luagame_tolight(L, -1));

			lua_pop(L, 1);
		}

		lua_pop(L, 1); // "lights"

		return env;
	}
}

int mesh_batch_begin(lua_State * L) {
	int args = lua_gettop(L);

	if (args < 3 || args > 4)
		return luaL_error(L, "invalid number of arguments to batch:begin()");

	auto batch = luagame_checkobj<mesh_batch>(L, 1);

	mat4 view_matrix = luagame_tomat4(L, 2);
	mat4 projection_matrix = luagame_tomat4(L, 3);

	environment_object env = args > 3 ? luagame_toenvironment(L, 4) : environment_object();

	batch->begin(env, view_matrix, projection_matrix);

	return 0;
}

int mesh_batch_add(lua_State * L) {
	if (lua_gettop(L) != 3) return luaL_error(L, "invalid number of arguments to batch:add()");

	auto batch = luagame_checkobj<mesh_batch>(L, 1);
	auto mesh = luagame_checkobj<mesh_object>(L, 2);
	mat4 model_matrix = luagame_tomat4(L, 3);

	batch->add_instance(mesh, model_matrix);

	return 0;
}

int mesh_batch_flush(lua_State * L) {
	if (lua_gettop(L) != 1) return luaL_error(L, "invalid number of arguments to batch:flush()");

	auto batch = luagame_checkobj<mesh_batch>(L, 1);
	batch->flush();

	return 0;
}

}