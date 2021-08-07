#pragma once

#include <unordered_map>
#include <functional>
#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <cmath>
#include <map>

#include <Physics3D/math/linalg/vec.h>
#include <Physics3D/math/linalg/mat.h>
#include <Physics3D/math/linalg/trigonometry.h>
#include <Physics3D/math/fix.h>
#include <Physics3D/math/position.h>

#include "../util/log.h"
#include <Physics3D/datastructures/smartPointers.h>

#define irepeat(x, n) for (int x = 0; x < n; x++)
#define frepeat(x, n) for (float x = 0.0f; x < n; x+=1.0f)
#define isrepeat(x, n, s) for (int x = 0; x < n; x+=s)
#define fsrepeat(x, n, s) for (float x = 0.0f; x < n; x+=s)

#ifdef _MSC_VER
	#define P3D_DEBUGBREAK __debugbreak()
#else
#include <csignal>
	#define P3D_DEBUGBREAK raise(SIGTRAP)
#endif
