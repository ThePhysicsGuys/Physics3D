#pragma once

#include "../physics/math/ray.h"

class Screen;

Ray getMouseRay(Screen& screen, Vec2f mouse);
