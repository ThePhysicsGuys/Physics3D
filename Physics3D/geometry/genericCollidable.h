#pragma once

#include "../math/linalg/vec.h"

namespace P3D {
struct GenericCollidable {
	virtual Vec3f furthestInDirection(const Vec3f& direction) const = 0;
};
};
