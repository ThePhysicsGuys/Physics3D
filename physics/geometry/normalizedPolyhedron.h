#pragma once

#include "polyhedron.h"
#include "normalizedShape.h"

class NormalizedPolyhedron : public ShapeClass, public Polyhedron {
public:
	NormalizedPolyhedron(Polyhedron&& poly) : Polyhedron(std::move(poly)), ShapeClass(poly.getVolume(), poly.getCenterOfMass(), poly.getScalableInertia(CFrame(poly.getCenterOfMass()))) {}
	NormalizedPolyhedron(const Polyhedron& poly) : Polyhedron(poly), ShapeClass(poly.getVolume(), poly.getCenterOfMass(), poly.getScalableInertia(CFrame(poly.getCenterOfMass()))) {}

	virtual bool containsPoint(Vec3 point) const override {
		return Polyhedron::containsPoint(point);
	}
	virtual double getIntersectionDistance(Vec3 origin, Vec3 direction) const override {
		return Polyhedron::getIntersectionDistance(origin, direction);
	}
	virtual BoundingBox getBounds() const override {
		return Polyhedron::getBounds();
	}
	virtual BoundingBox getBounds(const Mat3& referenceFrame) const override {
		return Polyhedron::getBounds(referenceFrame);
	}
	virtual double getMaxRadius() const override {
		return Polyhedron::getMaxRadius();
	}
	virtual double getMaxRadiusSq() const override {
		return Polyhedron::getMaxRadiusSq();
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
