#include "lgapi_gfx.h"
#include "lgapi_math.h"

using namespace luagame;

namespace {
	int set_texture(lua_State * L);
	int append(lua_State * L);
	int draw(lua_State * L);
}

void luagame_pushmesh(lua_State * L, mesh_object * mesh_object) {
	luagame_pushref(L, mesh_object);

	if (luaL_newmetatable(L, LUAGAME_MESH)) {
		luaL_Reg functions[] = {
			{ "set_texture", &set_texture },
			{ "append", &append },
			{ "draw", &draw },

			{ LUA_METAMETHOD_GC,	&lgapi_releaseref },
			{ NULL, NULL }
		};

		luaL_setfuncs(L, functions, NULL);
		lua_pushvalue(L, -1);
		lua_setfield(L, -1, LUA_METAMETHOD_INDEX);
	}

	lua_setmetatable(L, -2);
}

int lgapi_create_mesh(lua_State * L) {
	material::options opts = luagame_tomaterialoptions(L, 1);

	mesh_object * mesh_object = new luagame::mesh_object();

	mesh_object->acquire();
	mesh_object->set_material(opts);

	luagame_pushmesh(L, mesh_object);
	return 1;
}

mesh_object * luagame_tomesh(lua_State * L, int idx) {
	return luagame_unwrapref<mesh_object>(luaL_checkudata(L, idx, LUAGAME_MESH));
}

material::options luagame_tomaterialoptions(lua_State * L, int idx) {
	material::options opts = {};

	opts.use_position = true;

	if (lua_istable(L, idx)) {
		lua_getfield(L, idx, LUAGAME_MTLOPT_COLOR);
		opts.use_color = lua_toboolean(L, -1);

		lua_getfield(L, idx, LUAGAME_MTLOPT_TEXTURE);
		opts.use_texture = lua_toboolean(L, -1);

		lua_getfield(L, idx, LUAGAME_MTLOPT_NORMALS);
		opts.use_normal = lua_toboolean(L, -1);

		lua_getfield(L, idx, LUAGAME_MTLOPT_TRANSPARENCY);
		opts.use_transparency = lua_toboolean(L, -1);

		lua_pop(L, 4);
	}

	return opts;
}

namespace {
	int set_texture(lua_State * L) {
		mesh_object * mesh = luagame_tomesh(L, 1);


		if (lua_isstring(L, 2)) {
			mesh->set_texture(lua_tostring(L, 2));
		} else if (luagame_istexture(L, 2)) {
			mesh->set_texture(luagame_totexture(L, 2));
		} else {
			luaL_argerror(L, 2, "expected filename or texture object");
		}

		return 0;
	}

	int append(lua_State * L) {
		mesh_object * mesh = luagame_tomesh(L, 1);

		luaL_checktype(L, 2, LUA_TTABLE);

		lua_len(L, 2);
		int length = lua_tointeger(L, -1);

		_log("appending %d vertices", length);

		vertex * vertices = new vertex[length];

		for (int i = 0; i < length; i++) {
			lua_geti(L, 2, i + 1);
			vertices[i] = luagame_checkvertex(L, -1);
			lua_pop(L, 1);
		}
		
		mesh->append(vertices, length);

		_log("finished appending", length);

		delete[] vertices;

		return 0;
	}

	int draw(lua_State * L) {
		mesh_object * mesh = luagame_tomesh(L, 1);

		glm::mat4 model = luagame_tomat4(L, 2);
		glm::mat4 view = luagame_tomat4(L, 3);
		glm::mat4 proj = luagame_tomat4(L, 4);

		mesh->draw(model, view, proj);

		return 0;
	}
}