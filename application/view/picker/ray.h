#pragma once

#include "../engine/math/ray.h"

class Screen;

Ray getMouseRay(Screen& screen, Vec2f mouse);
