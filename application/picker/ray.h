#pragma once

#include "../physics/math/ray.h"

namespace Application {

class Screen;

Ray getMouseRay(Screen& screen, Vec2f mouse);

};