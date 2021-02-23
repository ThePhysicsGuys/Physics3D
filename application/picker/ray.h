#pragma once

#include "../physics/math/ray.h"

namespace P3D::Application {

class Screen;

Ray getMouseRay(const Screen& screen, const Vec2f& mouse);

};