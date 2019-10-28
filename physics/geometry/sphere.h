#pragma once

#include "genericCollidable.h"
#include "boundingBox.h"
#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"

class Polyhedron;

struct Sphere : public GenericCollidable {
	double radius;

	Sphere(double radius) : radius(radius) {}

	virtual Vec3f furthestInDirection(const Vec3f& direction) const;
	bool containsPoint(Vec3 point) const;
	double getIntersectionDistance(Vec3 origin, Vec3 direction) const;
	double getVolume() const;

	BoundingBox getBounds() const;
	BoundingBox getBounds(const Mat3& referenceFrame) const;
	Vec3 getCenterOfMass() const;
	DiagonalMat3 getInertia() const;
	double getMaxRadius() const;
	double getMaxRadiusSq() const;

	Polyhedron asPolyhedron() const;
};
