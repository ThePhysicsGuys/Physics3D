#pragma once

#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "boundingBox.h"
#include "genericCollidable.h"
#include "scalableInertialMatrix.h"

// a ShapeClass is defined as a shape with dimentions -1..1 in all axes. All functions work on scaled versions of the shape. 
// examples include: 
//    Sphere of radius=1
//    Cube of 2x2x2
//    Custom polygon of 2x2x2
class ShapeClass : public GenericCollidable {
public:
	const double volume;
	const Vec3 centerOfMass;
	const ScalableInertialMatrix inertia;
	const int classID;

	ShapeClass(double volume, Vec3 centerOfMass, ScalableInertialMatrix inertia, int classID);
	ShapeClass(double volume, Vec3 centerOfMass, ScalableInertialMatrix inertia);

	virtual bool containsPoint(Vec3 point) const = 0;
	virtual double getIntersectionDistance(Vec3 origin, Vec3 direction) const = 0;

	virtual BoundingBox getBounds() const = 0;
	virtual BoundingBox getBounds(const Mat3& referenceFrame) const = 0;
	virtual double getScaledMaxRadius(DiagonalMat3 scale) const = 0;
	virtual double getScaledMaxRadiusSq(DiagonalMat3 scale) const = 0;

	virtual Vec3f furthestInDirection(const Vec3f& direction) const = 0;

	virtual Polyhedron asPolyhedron() const = 0;

	static const ShapeClass* getShapeClassForId(int classID);
};
