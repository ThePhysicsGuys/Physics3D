#pragma once

#include "../math/linalg/vec.h"
#include "../math/cframe.h"
#include "genericCollidable.h"

class Shape;
class Polyhedron;

struct Intersection {
	bool intersects;
	// Local to first
	Vec3 intersection;
	// Local to first
	Vec3 exitVector;

	Intersection() : intersects(false) {}
	Intersection(const Vec3& intersection, const Vec3& exitVector) :
		intersects(true),
		intersection(intersection),
		exitVector(exitVector) {}
};

Intersection intersectsTransformed(const Shape& first, const Shape& second, const CFrame& relativeTransform);
Intersection intersectsTransformed(const GenericCollidable& first, const GenericCollidable& second, const CFrame& relativeTransform);


