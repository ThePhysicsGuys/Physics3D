#pragma once

#include <catch.h>
#include "../engine/math/vec3.h"

#define REQUIRE_VEC(first, second) REQUIRE(first.x == Approx(second.x)); REQUIRE(first.y == Approx(second.y)); REQUIRE(first.z == Approx(second.z)); 
#define REQUIRE_VEC_ZERO(vec) REQUIRE_VEC(vec, Vec3(0,0,0));
#define REQUIRE_ZERO(val) REQUIRE(val == Approx(0.0))