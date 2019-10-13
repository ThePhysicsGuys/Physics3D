#pragma once

#include "genericCollidable.h"

class Sphere : public GenericCollidable {
	double radius;

	virtual Vec3f furthestInDirection(const Vec3f& direction) const;
};
