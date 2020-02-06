#pragma once

#include <optional>

#include "../math/linalg/vec.h"
#include "../math/cframe.h"
#include "genericCollidable.h"

class Shape;
class Polyhedron;

struct Intersection {
	// Local to first
	Vec3 intersection;
	// Local to first
	Vec3 exitVector;

	Intersection(const Vec3& intersection, const Vec3& exitVector) :
		intersection(intersection),
		exitVector(exitVector) {}
};

std::optional<Intersection> intersectsTransformed(const Shape& first, const Shape& second, const CFrame& relativeTransform);
std::optional<Intersection> intersectsTransformed(const GenericCollidable& first, const GenericCollidable& second, const CFrame& relativeTransform, const DiagonalMat3& scaleFirst, const DiagonalMat3& scaleSecond);


