#pragma once


#include "boundingBox.h"
#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/cframe.h"
#include "../math/transform.h"
#include "shapeClass.h"

#include <iosfwd>

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

	inline void setWidth(double newWidth) { scale[0] = newWidth / 2; }
	inline void setHeight(double newHeight) { scale[1] = newHeight / 2; }
	inline void setDepth(double newDepth) { scale[2] = newDepth / 2; }

	Shape scaled(double scaleX, double scaleY, double scaleZ) const;
	
	BoundingBox getBounds() const;
	BoundingBox getBounds(const Mat3& referenceFrame) const;
	Vec3 getCenterOfMass() const;
	SymmetricMat3 getInertia() const;
	double getMaxRadius() const;
	double getMaxRadiusSq() const;

	Vec3f furthestInDirection(const Vec3f& direction) const;

	Polyhedron asPolyhedron() const;

	void serialize(std::ostream& ostream) const;
	static Shape deserialize(std::istream& istream);
};


