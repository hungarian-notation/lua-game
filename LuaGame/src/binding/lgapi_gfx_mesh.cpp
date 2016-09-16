#include "lgapi_gfx.h"
#include "lgapi_math.h"

using namespace luagame;

namespace {
	int set_texture(lua_State * L);
	int append(lua_State * L);
	int draw(lua_State * L);
}

void luagame_pushmesh(lua_State * L, meshptr mesh) {
	luaL_Reg functions[] = {
		{ "set_texture", &set_texture },
		{ "append", &append },
		{ "draw", &draw },

		{ NULL, NULL }
	};

	luagame_pushobj<mesh_object>(L, mesh, functions);
}

int lgapi_create_mesh(lua_State * L) {
	material_object::options opts = luagame_tomaterialoptions(L, 1);
	auto mesh = std::make_shared<luagame::mesh_object>();
	mesh->set_material(opts, luagame_getcontext(L)->material_cache);
	luagame_pushmesh(L, mesh);

	return 1;
}

material_object::options luagame_tomaterialoptions(lua_State * L, int idx) {
	material_object::options opts = {};

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
		meshptr mesh = luagame_checkobj<mesh_object>(L, 1);

		if (lua_isstring(L, 2)) {
			mesh->set_texture(lua_tostring(L, 2), luagame_getcontext(L)->texture_cache);
		} else if (luagame_isobj<texture_object>(L, 2)) {
			mesh->set_texture(luagame_checkobj<texture_object>(L, 2));
		} else {
			luaL_argerror(L, 2, "expected filename or texture object");
		}

		return 0;
	}

	int append(lua_State * L) {
		meshptr mesh = luagame_checkobj<mesh_object>(L, 1);

		luaL_checktype(L, 2, LUA_TTABLE);

		lua_len(L, 2);
		int length = (int)lua_tointeger(L, -1);

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
		meshptr mesh = luagame_checkobj<mesh_object>(L, 1);

		glm::mat4 model = luagame_tomat4(L, 2);
		glm::mat4 view = luagame_tomat4(L, 3);
		glm::mat4 proj = luagame_tomat4(L, 4);

		mesh->draw(model, view, proj);

		return 0;
	}
}