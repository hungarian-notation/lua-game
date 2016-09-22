
import re

def to_array_initializer(binary_data, terminator = "'\\x00' /* NULL */", columns = 12, indent = 4):
  """ convert the binary string into a c-style array initializer """

  output = ""
  terminated = not (terminator is None)
  for i in range(0, len(binary_data)):
    if (not (columns is None) and i % columns == 0): 
      output += "\n" + " " * indent
    hex_string = hex(binary_data[i])
    hex_string = re.match("[0][x]([0-9a-f]+){1,2}", hex_string).group(1)
    hex_string = '0' * (2 - len(hex_string)) + hex_string
    term = "'\\x{HEX_DIGITS}'".format(HEX_DIGITS = hex_string);
    if ((terminated and i < len(binary_data)) or 
        (not terminated and i < len(binary_data) - 1)): 
      term += ", "
    output += "{0}".format(term)
  if terminated: output += terminator
  return output;

def generate_header_source(source_path, resource_name, namespace = None, null_terminated = True):
  """ compiles the file at source_path into a C++ header file """

  if namespace is None:
    namespace = [None]
  if isinstance(namespace, str):
    namespace = [namespace]
    
  source_file = open(source_path, "rb")
  source_data = source_file.read()
  source_file.close()

  terminator = None;
  if null_terminated: terminator = "'\\x00'";

  open_namespace = ""
  close_namespace = ""

  for name in namespace:
    open_namespace = open_namespace + "\n" + "namespace {0} {{".format(name)
    close_namespace = "}} // {0}\n".format(name) + close_namespace

  header_source = """// GENERATED FILE; DO NOT EDIT
// generated from: {SOURCE_PATH}

#pragma once 
{OPEN_NAMESPACE}

const char {RESOURCE_NAME}[] = {{{ARRAY_CONTENTS}
}};

{CLOSE_NAMESPACE}

""";

  header_source = header_source.format(
    SOURCE_PATH = source_path,
    RESOURCE_NAME = resource_name,
    ARRAY_CONTENTS = to_array_initializer(source_data, terminator),
    OPEN_NAMESPACE = open_namespace,
    CLOSE_NAMESPACE = close_namespace
  );

  return header_source;


def include_file(source_path, resource_name, namespace = None, null_terminated = True, header_path = None):
  """ compiles the file at (source_path) into a C++ header file and writes it to (header_path) """
 
  print("translating resource: {RESOURCE_NAME}".format(RESOURCE_NAME = resource_name));

  if header_path is None: header_path = "src/res/{NAME}.res.h".format(NAME = resource_name)

  header_source = generate_header_source(source_path, resource_name, namespace, null_terminated)

  header_file = open(header_path, "w")
  header_file.write(header_source)
  header_file.close()