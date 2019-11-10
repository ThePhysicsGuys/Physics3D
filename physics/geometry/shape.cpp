#include "shape.h"

#include "polyhedron.h"
#include "normalizedPolyhedron.h"
#include "sphere.h"

Shape::Shape() : baseShape(nullptr), scale{1,1,1} {}
Shape::Shape(const ShapeClass* baseShape, DiagonalMat3 scale) : baseShape(baseShape), scale(scale) {}
Shape::Shape(const ShapeClass* baseShape) : baseShape(baseShape), scale{1,1,1} {}
Shape::Shape(const ShapeClass* baseShape, double width, double height, double depth) : baseShape(baseShape), scale{width / 2, height / 2, depth / 2} {}
Shape::Shape(const Polyhedron& baseShape) : baseShape(new NormalizedPolyhedron(baseShape)), scale{1,1,1} {}
Shape::Shape(Polyhedron&& baseShape) : baseShape(new NormalizedPolyhedron(std::move(baseShape))), scale{1,1,1} {}

bool Shape::containsPoint(Vec3 point) const {
	return baseShape->containsPoint(~scale * point);
}
double Shape::getIntersectionDistance(Vec3 origin, Vec3 direction) const {
	return baseShape->getIntersectionDistance(~scale * origin, ~scale * direction);
}
double Shape::getVolume() const {
	return baseShape->volume * det(scale);
}
Shape Shape::scaled(double scaleX, double scaleY, double scaleZ) const {
	return Shape(baseShape, scale * DiagonalMat3{scaleX, scaleY, scaleZ});
}
BoundingBox Shape::getBounds() const {
	return baseShape->getBounds().scaled(scale[0], scale[1], scale[2]);
}
BoundingBox Shape::getBounds(const Mat3& referenceFrame) const {
	return baseShape->getBounds(referenceFrame * scale);
}
Vec3 Shape::getCenterOfMass() const {
	return scale * baseShape->centerOfMass;
}
SymmetricMat3 Shape::getInertia() const {
	return baseShape->inertia.toMatrix(scale);
}
double Shape::getMaxRadius() const {
	return baseShape->getScaledMaxRadius(scale);
}
double Shape::getMaxRadiusSq() const {
	return baseShape->getScaledMaxRadiusSq(scale);
}
Vec3f Shape::furthestInDirection(const Vec3f& direction) const {
	return baseShape->furthestInDirection(direction);
}

Polyhedron Shape::asPolyhedron() const {
	return baseShape->asPolyhedron();
}
