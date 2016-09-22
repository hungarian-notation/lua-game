from auxiliaries import compile_auxiliaries

options = {
  'auxiliaries_dir'       : "auxlib",
  'auxiliaries_header'    : "src/binding/lgapi_aux.gen.h",
  'auxiliaries_loadfunc'  : "luagame_loadauxlib",
  'luac_path'             : "Z:/libs/cpp/lua/lua-5.3.3/bin/luac.exe",
  'lua_header'            : "lua53/lua.hpp"
}

compile_auxiliaries(options)