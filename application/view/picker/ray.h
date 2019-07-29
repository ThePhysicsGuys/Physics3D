#pragma once

#include "../engine/math/vec2.h"
#include "../engine/math/vec3.h"

class Screen;

struct Ray {
	Vec3f start;
	Vec3f direction;
};

Ray getMouseRay(Screen& screen, Vec2f mouse);
