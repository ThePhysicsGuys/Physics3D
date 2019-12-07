#pragma once


#include "boundingBox.h"
#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/cframe.h"
#include "../math/transform.h"

class ShapeClass;
class Polyhedron;

class Shape {
	Shape(const ShapeClass* baseShape, DiagonalMat3 scale);
public:
	const ShapeClass* baseShape;
	DiagonalMat3 scale;
public:
	Shape();
	Shape(const ShapeClass* baseShape);
	Shape(const ShapeClass* baseShape, double width, double height, double depth);

	bool containsPoint(Vec3 point) const;
	double getIntersectionDistance(Vec3 origin, Vec3 direction) const;
	double getVolume() const;


	inline double getWidth() const { return scale[0] * 2; }
	inline double getHeight() const { return scale[1] * 2; }
	inline double getDepth() const { return scale[2] * 2; }

	void setWidth(double newWidth);
	void setHeight(double newHeight);
	void setDepth(double newDepth);

	Shape scaled(double scaleX, double scaleY, double scaleZ) const;
	
	BoundingBox getBounds() const;
	BoundingBox getBounds(const Mat3& referenceFrame) const;
	Vec3 getCenterOfMass() const;
	// defined around the object's Center Of Mass
	SymmetricMat3 getInertia() const;
	double getMaxRadius() const;
	double getMaxRadiusSq() const;

	Vec3f furthestInDirection(const Vec3f& direction) const;

	Polyhedron asPolyhedron() const;
};


