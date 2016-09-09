#pragma once

#include <glm\vec3.hpp>
#include <glm\vec4.hpp>
#include <glm\mat4x4.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>

// What the hell? 
// minwindef.h defines 'near' and 'far'?

#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif