#pragma once

#include <unordered_map>
#include <functional>
#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <cmath>
#include <map>

#include "../physics/math/linalg/vec.h"
#include "../physics/math/fix.h"
#include "../physics/math/linalg/mat.h"
#include "../physics/math/linalg/trigonometry.h"
#include "../physics/math/position.h"

#include "../util/log.h"
#include "../util/intrusivePointer.h"

#define irepeat(x, n) for (int x = 0; x < n; x++)
#define frepeat(x, n) for (float x = 0.0f; x < n; x+=1.0f)
#define isrepeat(x, n, s) for (int x = 0; x < n; x+=s)
#define fsrepeat(x, n, s) for (float x = 0.0f; x < n; x+=s)