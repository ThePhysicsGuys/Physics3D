#pragma once

#include <string>

#define BASIC_SHADER 0
#define BASICNORMAL_SHADER 1
#define VECTOR_SHADER 2
#define ORIGIN_SHADER 3
#define OUTLINE_SHADER 4
#define QUAD_SHADER 5
#define STALL_MODEL 6

std::string getResourceAsString(int res);
