#pragma once

#include <assert.h>
#include <math.h>

#define CHECK_VALID_VEC(vec) assert(isfinite(lengthSquared(vec)))
