#pragma once

#include "vec3.h"
#include "position.h"

struct Ray {
	Position start;
	Vec3 direction;
};

/*bool doRayAndBoundsIntersect(const Bounds& bounds, const Ray& ray) {

}*/
