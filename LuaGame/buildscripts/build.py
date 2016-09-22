from auxiliaries import compile_auxiliaries
from include_file import include_file

compile_auxiliaries({
  'auxiliaries_dir'       : "auxlib",
  'auxiliaries_header'    : "src/binding/lgapi_aux.gen.h",
  'auxiliaries_loadfunc'  : "luagame_loadauxlib",
  'luac_path'             : "Z:/libs/cpp/lua/lua-5.3.3/bin/luac.exe",
  'lua_header'            : "lua53/lua.hpp"
})

resource_namespace = [ "luagame", "resources" ]

include_file("shaders/fragment.glsl", "fragment_glsl",  resource_namespace);
include_file("shaders/vertex.glsl",   "vertex_glsl",    resource_namespace);