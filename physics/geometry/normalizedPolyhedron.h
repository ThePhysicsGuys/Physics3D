#pragma once

#include "polyhedron.h"
#include "shapeClass.h"

class NormalizedPolyhedron : public ShapeClass, public Polyhedron {
	friend class Polyhedron;
	NormalizedPolyhedron(Polyhedron&& poly, Vec3 originalCenter, DiagonalMat3 originalScale, double volume, Vec3 localCenterOfMass, ScalableInertialMatrix inertia) : 
		Polyhedron(std::move(poly)), ShapeClass(volume, localCenterOfMass, inertia, CONVEX_POLYHEDRON_CLASS_ID), originalCenter(originalCenter), originalScale(originalScale) {}
public:
	const Vec3 originalCenter;
	const DiagonalMat3 originalScale;
	virtual bool containsPoint(Vec3 point) const override {
		return Polyhedron::containsPoint(point);
	}
	virtual double getIntersectionDistance(Vec3 origin, Vec3 direction) const override {
		return Polyhedron::getIntersectionDistance(origin, direction);
	}
	virtual BoundingBox getBounds(const Rotation& rotation, const DiagonalMat3& scale) const override {
		return Polyhedron::getBounds(Mat3f(rotation.asRotationMatrix() * scale));
	}
	virtual double getScaledMaxRadius(DiagonalMat3 scale) const override {
		return Polyhedron::getScaledMaxRadius(scale);
	}
	virtual double getScaledMaxRadiusSq(DiagonalMat3 scale) const override {
		return Polyhedron::getScaledMaxRadiusSq(scale);
	}

	virtual Vec3f furthestInDirection(const Vec3f& direction) const override {
		return Polyhedron::furthestInDirection(direction);
	}

	virtual Polyhedron asPolyhedron() const override {
		return static_cast<Polyhedron>(*this);
	}
};
