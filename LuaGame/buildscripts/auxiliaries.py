import os
import re
from subprocess import call 
from shutil     import copyfile

def find_luas(dir, targets = None):
  if targets is None: targets = []

  items = os.listdir(dir);

  for item in items:
    full_path = os.path.join(dir, item);

    if os.path.isdir(full_path):
      find_luas(full_path, targets);
    elif os.path.isfile(full_path) and item.endswith(".lua"):
      targets.append(full_path);

  return targets;


def compile_auxiliaries(opts):

  print( "building auxiliary library..." );

  # compile the auxiliary library with luac

  targets = find_luas(opts['auxiliaries_dir'])
  
  command = [opts['luac_path']]
  
  for target in targets:
    print("  found: {target}".format(target = target))
    command.append(target)

  call(command)

  # read generated lua bytecode

  generated_file = open("luac.out", 'rb')
  binary_data = generated_file.read()
  generated_file.close()

  os.remove("luac.out")

  # convert bytecode into a binary string constant

  arr_initializer = ""

  for i in range(0, len(binary_data)):
    if (i % 8 == 0): 
      arr_initializer += "\n  "

    term = hex(binary_data[i])
    
    if (i < len(binary_data) - 1): 
      term += ", "

    term = "(char)" + term + " " * (6 - len(term))

    arr_initializer += "{0}".format(term)

  # write the header file


  header_preamble = "// THIS FILE WAS GENERATED BY A SCRIPT - DO NOT EDIT IT DIRECTLY\n// GENERATED FROM {aux_entry}\n".format(
    aux_entry= opts['auxiliaries_dir']
  );

  header_source = """
{PREAMBLE}

#pragma once
#include <{LUA_HEADER}>

int {LOAD_FUNCTION} (lua_State * L) {{
  const char buffer[] = {{{BUFFER}
  }};

  return luaL_loadbufferx(L, buffer, sizeof(buffer), "auxiliary library", "b");
}};
  """;

  header_source = header_source.format(
    PREAMBLE      = header_preamble,
    LUA_HEADER    = opts['lua_header'],
    LOAD_FUNCTION = opts['auxiliaries_loadfunc'],
    BUFFER        = arr_initializer
  ); 

  header_file = open(opts['auxiliaries_header'], "w");
  header_file.write(header_source);
  header_file.close()

  print( "done." );

