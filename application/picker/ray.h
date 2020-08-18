#pragma once

#include "../physics/math/ray.h"

namespace P3D::Application {

class Screen;

Ray getMouseRay(Screen& screen, Vec2f mouse);

};