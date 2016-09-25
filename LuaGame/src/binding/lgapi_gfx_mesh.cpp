#include "lgapi_gfx.h"
#include "lgapi_math.h"
#include "..\graphics\mesh_batch.h"

using namespace luagame;
using namespace glm;


namespace {
int set_texture(lua_State * L);
int set_material(lua_State * L);
int append(lua_State * L);
int append_quad(lua_State * L);
int draw(lua_State * L);
}

void luagame_pushmesh(lua_State * L, meshptr mesh) {
	luaL_Reg functions[] = {
		{ "set_texture", &set_texture },
		{ "set_material", &set_material },
		{ "append", &append },
		// { "append_quad", &append_quad },
		{ "draw", &draw },

		{ NULL, NULL }
	};

	luagame_pushobj<mesh_object>(L, mesh, functions);
}

int lgapi_create_mesh(lua_State * L) {
	material_object::material_options opts = luagame_tomaterialoptions(L, 1);

	auto mesh = std::make_shared<luagame::mesh_object>(&(luagame_getcontext(L)->material_cache), &(luagame_getcontext(L)->texture_cache));
	
	mesh->set_material(opts);

	luagame_pushmesh(L, mesh);

	return 1;
}

namespace {
int set_texture(lua_State * L) {
	meshptr mesh = luagame_checkobj<mesh_object>(L, 1);

	if (lua_isstring(L, 2)) {
		mesh->set_texture(lua_tostring(L, 2));
	} else if (luagame_isobj<texture_object>(L, 2)) {
		mesh->set_texture(luagame_checkobj<texture_object>(L, 2));
	} else {
		luaL_argerror(L, 2, "expected filename or texture object");
	}

	return 0;
}

int set_material(lua_State * L) {
	auto mesh = luagame_checkobj<mesh_object>(L, 1);

	if (luagame_isobj<material_object>(L, 2)) {
		mesh->set_material(luagame_checkobj<material_object>(L, 2));
	} else if (lua_istable(L, 2)) {
		mesh->set_material(luagame_tomaterialoptions(L, 2));
	} else {
		luaL_argerror(L, 2, "expected material or material options");
	}

	return 0;
}

int append(lua_State * L) {
	if (lua_gettop(L) < 2)
		return luaL_error(L, "not enough arguments; expected mesh:append({ vertices })");
	if (lua_gettop(L) > 2)
		return luaL_error(L, "too many arguments; expected mesh:append({ vertices })");
	
	meshptr mesh = luagame_checkobj<mesh_object>(L, 1);

	luaL_checktype(L, 2, LUA_TTABLE);

	lua_len(L, 2);

	int length = (int)lua_tointeger(L, -1);

	vertex * vertices = new vertex[length];

	for (int i = 0; i < length; i++) {
		lua_geti(L, 2, i + 1);
		vertices[i] = luagame_checkvertex(L, -1);
		lua_pop(L, 1);
	}

	mesh->append(vertices, length);

	delete[] vertices;

	return 0;

}

int append_quad(lua_State * L) {
	return 0;
}


int draw(lua_State * L) {
	meshptr mesh = luagame_checkobj<mesh_object>(L, 1);

	mat4 model = luagame_tomat4(L, 2);
	mat4 view = luagame_tomat4(L, 3);
	mat4 projection = luagame_tomat4(L, 4);

	auto env = environment_object(vec3(0.2F));

	env.add_light(light(vec3(0, 0, 0), 30, vec3(0.8, 0.0, 0.0)));
	env.add_light(light(vec3(1, 0, 1), vec3(0.6, 0.6, 0.6)));

	mesh_batch batch;

	batch.begin(env, view, projection);
	batch.add_instance(mesh, model);
	batch.flush();

	return 0;
}
}