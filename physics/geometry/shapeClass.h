#pragma once

#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/rotation.h"
#include "boundingBox.h"
#include "genericCollidable.h"
#include "scalableInertialMatrix.h"

#define CUBE_CLASS_ID 0
#define SPHERE_CLASS_ID 1
#define CYLINDER_CLASS_ID 2
#define CONVEX_POLYHEDRON_CLASS_ID 10
class Polyhedron;

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
	const int intersectionClassID;

	ShapeClass(double volume, Vec3 centerOfMass, ScalableInertialMatrix inertia, int intersectionClassID);

	virtual bool containsPoint(Vec3 point) const = 0;
	virtual double getIntersectionDistance(Vec3 origin, Vec3 direction) const = 0;

	virtual BoundingBox getBounds(const Rotation& referenceFrame, const DiagonalMat3& scale) const = 0;

	virtual double getScaledMaxRadius(DiagonalMat3 scale) const;
	virtual double getScaledMaxRadiusSq(DiagonalMat3 scale) const = 0;

	virtual Vec3f furthestInDirection(const Vec3f& direction) const = 0;

	virtual Polyhedron asPolyhedron() const = 0;

	// these functions determine the relations between the axes, for example, for Sphere, all axes must be equal
	virtual void setScaleX(double newX, DiagonalMat3& scale) const;
	virtual void setScaleY(double newY, DiagonalMat3& scale) const;
	virtual void setScaleZ(double newZ, DiagonalMat3& scale) const;
};

extern const ShapeClass* const sphereClass;
extern const ShapeClass* const boxClass;
extern const ShapeClass* const cylinderClass;
