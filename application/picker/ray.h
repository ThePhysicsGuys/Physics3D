#pragma once

#include <Physics3D/math/ray.h>

namespace P3D::Application {

class Screen;

Ray getMouseRay(const Screen& screen, const Vec2f& mouse);

};