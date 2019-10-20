#pragma once


#include "boundingBox.h"
#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/cframe.h"
#include "../math/transform.h"

enum class ShapeType : size_t {
	BOX = 0,
	WEDGE = 1,
	SPHERE = 2,
	CYLINDER = 3,
	CONE = 4,
};

class Shape {
public:
	union {
		const Polyhedron* polyhedron;
		ShapeType type; // we assume that the polyhedron * will never be in the range of ShapeType, let's hope that's the case
	};
public:
	double width;
	union {
		double height;
		double radius;
	};
	double depth;
public:
	Shape();
	Shape(const Polyhedron* polyhedron);

	Shape(const Polyhedron& polyhedron);
	Shape(Polyhedron&& polyhedron);

	bool containsPoint(Vec3f point) const;
	float getIntersectionDistance(Vec3f origin, Vec3f direction) const;
	double getVolume() const;

	Shape scaled(float scaleX, float scaleY, float scaleZ) const;
	Shape scaled(double scaleX, double scaleY, double scaleZ) const;
	
	BoundingBox getBounds() const;
	BoundingBox getBounds(const Mat3& referenceFrame) const;
	Vec3 getCenterOfMass() const;
	SymmetricMat3 getInertia() const;
	CircumscribingSphere getCircumscribingSphere() const;
	double getMaxRadius() const;
	double getMaxRadiusSq() const;

	Vec3f furthestInDirection(const Vec3f& direction) const;

	Polyhedron asPolyhedron() const;
};


