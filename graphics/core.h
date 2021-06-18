#pragma once

#include <unordered_map>
#include <functional>
#include <algorithm>
#include <optional>
#include <utility>
#include <string>
#include <vector>
#include <cmath>
#include <map>

#include <Physics3D/math/linalg/vec.h>
#include <Physics3D/math/fix.h>
#include <Physics3D/math/linalg/mat.h>
#include <Physics3D/math/linalg/trigonometry.h>
#include <Physics3D/math/position.h>
#include <Physics3D/datastructures/intrusivePointer.h>

#include "../util/log.h"

#ifdef _MSC_VER
#define P3D_DEBUGBREAK __debugbreak()
#else
#define P3D_DEBUGBREAK raise(SIGTRAP)
#endif