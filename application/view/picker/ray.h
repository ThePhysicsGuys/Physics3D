#pragma once

#include "../engine/math/vec.h"
#include "../engine/math/ray.h"

class Screen;

/*struct Ray {
	Vec3f start;
	Vec3f direction;
};*/

Ray getMouseRay(Screen& screen, Vec2f mouse);
