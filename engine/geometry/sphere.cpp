#include "sphere.h"

#include "../math/linalg/trigonometry.h"

Vec3f Sphere::furthestInDirection(const Vec3f& direction) const {
	return direction * (this->radius / length(direction));
}
