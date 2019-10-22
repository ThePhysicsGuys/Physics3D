#pragma once

#include "genericCollidable.h"
#include "boundingBox.h"
#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"

class Sphere : public GenericCollidable {
	double radius;

	virtual Vec3f furthestInDirection(const Vec3f& direction) const;

	bool containsPoint(const Vec3f& point) const;
	float getIntersectionDistance(const Vec3f& origin, const Vec3f& direction) const;
	double getVolume() const;

	BoundingBox getBounds() const;
	BoundingBox getBounds(const Mat3& referenceFrame) const;
	Vec3 getCenterOfMass() const;
	SymmetricMat3 getInertia() const;
	double getMaxRadius() const;
	double getMaxRadiusSq() const;

	Vec3f furthestInDirection(const Vec3f& direction) const;

	Polyhedron asPolyhedron() const;
};
