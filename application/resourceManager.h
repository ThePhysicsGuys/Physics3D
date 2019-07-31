#pragma once

#include <string>

#define BASIC_SHADER 0
#define VECTOR_SHADER 1
#define ORIGIN_SHADER 2
#define FONT_SHADER 3
#define DEPTH_SHADER 4
#define QUAD_SHADER 5
#define POSTPROCESS_SHADER 6
#define SKYBOX_SHADER 7
#define MASK_SHADER 8
#define POINT_SHADER 9
#define TEST_SHADER 10
#define BLUR_SHADER 11
#define COLORWHEEL_SHADER 12
#define TEST1_SHADER 13
#define EDGE_SHADER 14
#define SPHERE_MODEL 15
#define STALL_MODEL 16

std::string getResourceAsString(int res);
