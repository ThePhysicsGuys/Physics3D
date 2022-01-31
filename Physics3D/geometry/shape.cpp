#include "shape.h"

#include "polyhedron.h"
#include "shapeClass.h"

namespace P3D {
Shape::Shape() : baseShape(nullptr), scale{1,1,1} {}
Shape::Shape(intrusive_ptr<const ShapeClass> baseShape, DiagonalMat3 scale) : 
	baseShape(std::move(baseShape)), scale(scale) {
	assert(this->baseShape);
}

Shape::Shape(intrusive_ptr<const ShapeClass> baseShape) : 
	baseShape(std::move(baseShape)), scale{1,1,1} {
	assert(this->baseShape);
}

Shape::Shape(intrusive_ptr<const ShapeClass> baseShape, double width, double height, double depth) :
	baseShape(std::move(baseShape)), scale{width / 2, height / 2, depth / 2} {
	assert(this->baseShape);
}

// defined here so that ShapeClass destructor does not need to be called externally
Shape::~Shape() {}

Shape::Shape(Shape&&) = default;
Shape& Shape::operator=(Shape&&) = default;
Shape::Shape(const Shape&) = default;
Shape& Shape::operator=(const Shape&) = default;

bool Shape::containsPoint(Vec3 point) const {
	return baseShape->containsPoint(~scale * point);
}
double Shape::getIntersectionDistance(Vec3 origin, Vec3 direction) const {
	return baseShape->getIntersectionDistance(~scale * origin, ~scale * direction);
}
double Shape::getVolume() const {
	return baseShape->volume * det(scale);
}

Shape Shape::scaled(const DiagonalMat3& scale) const {
	return Shape(baseShape, this->scale * scale);
}
Shape Shape::scaled(double scaleX, double scaleY, double scaleZ) const {
	return Shape(baseShape, scale * DiagonalMat3{scaleX, scaleY, scaleZ});
}
BoundingBox Shape::getBounds() const {
	return BoundingBox{-scale[0], -scale[1], -scale[2], scale[0], scale[1], scale[2]};
}
BoundingBox Shape::getBounds(const Rotation& referenceFrame) const {
	return baseShape->getBounds(referenceFrame, scale);
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
	return baseShape->asPolyhedron().scaled(scale);
}

void Shape::setWidth(double newWidth) {
	baseShape->setScaleX(newWidth / 2, scale);
}
void Shape::setHeight(double newHeight) {
	baseShape->setScaleY(newHeight / 2, scale);
}
void Shape::setDepth(double newDepth) {
	baseShape->setScaleZ(newDepth / 2, scale);
}
};
