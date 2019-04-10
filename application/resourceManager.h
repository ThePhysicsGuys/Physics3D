#pragma once

#include <string>

#define BASIC_SHADER 0
#define VECTOR_SHADER 1
#define ORIGIN_SHADER 2
#define FONT_SHADER 3
#define BASICNORMAL_SHADER 4
#define QUAD_SHADER 5
#define POSTPROCESS_SHADER 6
#define SKYBOX_SHADER 7
#define SPHERE_MODEL 8
#define STALL_MODEL 9

std::string getResourceAsString(int res);
