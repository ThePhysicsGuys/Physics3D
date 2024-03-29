#pragma once


#include "../math/boundingBox.h"
#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/cframe.h"
#include "../math/transform.h"
#include "../datastructures/smartPointers.h"

namespace P3D {
class ShapeClass;
class Polyhedron;

class Shape {
public:
	intrusive_ptr<const ShapeClass> baseShape;
	DiagonalMat3 scale;

	Shape();
	Shape(intrusive_ptr<const ShapeClass> baseShape);
	Shape(intrusive_ptr<const ShapeClass> baseShape, DiagonalMat3 scale);
	Shape(intrusive_ptr<const ShapeClass> baseShape, double width, double height, double depth);
	~Shape();

	Shape(Shape&&);
	Shape& operator=(Shape&&);
	Shape(const Shape&);
	Shape& operator=(const Shape&);

	[[nodiscard]] bool containsPoint(Vec3 point) const;
	[[nodiscard]] double getIntersectionDistance(Vec3 origin, Vec3 direction) const;
	[[nodiscard]] double getVolume() const;

	[[nodiscard]] double getWidth() const { return scale[0] * 2; }
	[[nodiscard]] double getHeight() const { return scale[1] * 2; }
	[[nodiscard]] double getDepth() const { return scale[2] * 2; }

	void setWidth(double newWidth);
	void setHeight(double newHeight);
	void setDepth(double newDepth);

	[[nodiscard]] Shape scaled(const DiagonalMat3& scale) const;
	[[nodiscard]] Shape scaled(double scaleX, double scaleY, double scaleZ) const;

	[[nodiscard]] BoundingBox getBounds() const;
	[[nodiscard]] BoundingBox getBounds(const Rotation& referenceFrame) const;
	[[nodiscard]] Vec3 getCenterOfMass() const;
	// defined around the object's Center Of Mass
	[[nodiscard]] SymmetricMat3 getInertia() const;
	[[nodiscard]] double getMaxRadius() const;
	[[nodiscard]] double getMaxRadiusSq() const;

	[[nodiscard]] Vec3f furthestInDirection(const Vec3f& direction) const;

	[[nodiscard]] Polyhedron asPolyhedron() const;
};
};

