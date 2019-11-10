#pragma once


#include "boundingBox.h"
#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/cframe.h"
#include "../math/transform.h"
#include "normalizedShape.h"

class Shape {
	Shape(const ShapeClass* baseShape, DiagonalMat3 scale);
public:
	const ShapeClass* baseShape;
	DiagonalMat3 scale;
public:
	Shape();
	Shape(const ShapeClass* baseShape);
	Shape(const ShapeClass* baseShape, double width, double height, double depth);
	Shape(const Polyhedron& baseShape);
	Shape(Polyhedron&& baseShape);

	bool containsPoint(Vec3 point) const;
	double getIntersectionDistance(Vec3 origin, Vec3 direction) const;
	double getVolume() const;

	Shape scaled(double scaleX, double scaleY, double scaleZ) const;
	
	BoundingBox getBounds() const;
	BoundingBox getBounds(const Mat3& referenceFrame) const;
	Vec3 getCenterOfMass() const;
	SymmetricMat3 getInertia() const;
	double getMaxRadius() const;
	double getMaxRadiusSq() const;

	Vec3f furthestInDirection(const Vec3f& direction) const;

	Polyhedron asPolyhedron() const;
};


