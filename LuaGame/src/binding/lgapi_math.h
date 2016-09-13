#pragma once

#include "binding.h"
#include "..\graphics\graphics_math.h"

#define LUAGAME_VECTOR "luagame_vector"

// Pushes a vector onto the stack.
void luagame_pushvector(lua_State * L, glm::vec4 vector);

// Pushes a vector onto the stack.
void luagame_pushvector(lua_State * L, glm::vec3 vector, float w = 1);

// Pushes a vector onto the stack.
void luagame_pushvector(lua_State * L, glm::vec2 vector, float z = 0, float w = 0);

// Tests if the value at idx is a true vector.
int luagame_isvector(lua_State * L, int idx);

// Retrieves a pointer to the userdata of the true vector at idx.
glm::vec4 * luagame_tovector(lua_State * L, int idx);

// Retrieves the value of the vector or vector-like table at idx.
glm::vec4 luagame_tovec4(lua_State * L, int idx);

// Retrieves the value of the vector or vector-like table at idx.
glm::vec3 luagame_tovec3(lua_State * L, int idx);

// Retrieves the value of the vector or vector-like table at idx.
glm::vec2 luagame_tovec2(lua_State * L, int idx);

// Lua: Creates a new vector.
//
// create_vector ( [ x, [ y, [ z, [ w ]]]] )
// create_vector ( vector )
// create_vector ( vector_like )
int lgapi_create_vector(lua_State * L);




#define LUAGAME_MATRIX "luagame_matrix"

	// Pushes a matrix onto the stack.
void luagame_pushmatrix(lua_State * L, glm::mat4 matrix);

// Tests if the value at idx is a matrix.
int luagame_ismatrix(lua_State * L, int idx);

// Retrieves a pointer to the userdata of the matrix at idx.
glm::mat4 * luagame_tomatrix(lua_State * L, int idx);

// Retrieves the value of the matrix at idx.
glm::mat4 luagame_tomat4(lua_State * L, int idx);

// Lua: Creates a new matrix.
//
// create_matrix ( void )						-- creates an identity matrix
// create_matrix ( matrix )						-- creates a copy of the matrix
// create_matrix ( vec, vec, vec, vec )			-- creates a matrix from four column vectors
// create_matrix ( num1, num2, num3 ... num16 )	-- creates a matrix from 16 components in column-major order
int lgapi_create_matrix(lua_State * L);