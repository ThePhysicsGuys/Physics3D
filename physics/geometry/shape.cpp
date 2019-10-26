#include "shape.h"

#include "polyhedron.h"
#include "sphere.h"


/*#define CUSTOM_DISPATCH(func) \
switch(this->type) {\
case BOX: Box(width, height, depth).func; break;\
case WEDGE: Wedge(width, height, depth).func; break;\
case SPHERE: Sphere(radius).func; break;\
case CYLINDER: Cylinder(radius, depth).func; break;\
case CONE: Cone(radius, depth).func; break;\
default: this->polyhedron->func;\
}*/

#define CUSTOM_DISPATCH(func) \
switch(this->type) {\
case ShapeType::SPHERE: return Sphere(this->dimensions.width).func;\
default: return this->polyhedron->func;\
}


Shape::Shape() : polyhedron() {}
Shape::Shape(const Polyhedron* polyhedron) : polyhedron(polyhedron) {}
Shape::Shape(const Polyhedron& polyhedron) : polyhedron(new Polyhedron(polyhedron)) {}
Shape::Shape(Polyhedron&& polyhedron) : polyhedron(new Polyhedron(std::move(polyhedron))) {}
Shape::Shape(Sphere sphere) : dimensions{sphere.radius, sphere.radius,sphere.radius}, type(ShapeType::SPHERE) {}

bool Shape::containsPoint(Vec3f point) const {
	CUSTOM_DISPATCH(containsPoint(point));
}
float Shape::getIntersectionDistance(Vec3f origin, Vec3f direction) const {
	CUSTOM_DISPATCH(getIntersectionDistance(origin, direction));
}
double Shape::getVolume() const {
	CUSTOM_DISPATCH(getVolume());
}
Shape Shape::scaled(float scaleX, float scaleY, float scaleZ) const {
	return scaled(double(scaleX), double(scaleY), double(scaleZ));
}
Shape Shape::scaled(double scaleX, double scaleY, double scaleZ) const {
	switch(this->type) {
	case ShapeType::SPHERE:
		return *this;
	default:
		return Shape(new Polyhedron(polyhedron->scaled(scaleX, scaleY, scaleZ)));
	}
}
BoundingBox Shape::getBounds() const {
	CUSTOM_DISPATCH(getBounds());
}
BoundingBox Shape::getBounds(const Mat3& referenceFrame) const {
	CUSTOM_DISPATCH(getBounds(referenceFrame));
}
Vec3 Shape::getCenterOfMass() const {
	CUSTOM_DISPATCH(getCenterOfMass());
}
SymmetricMat3 Shape::getInertia() const {
	CUSTOM_DISPATCH(getInertia());
}
double Shape::getMaxRadius() const {
	CUSTOM_DISPATCH(getMaxRadius());
}
double Shape::getMaxRadiusSq() const {
	CUSTOM_DISPATCH(getMaxRadiusSq());
}
Vec3f Shape::furthestInDirection(const Vec3f& direction) const {
	CUSTOM_DISPATCH(furthestInDirection(direction));
}

Polyhedron Shape::asPolyhedron() const {
	switch(this->type) {
	case ShapeType::SPHERE:
		return Sphere(this->dimensions.width).asPolyhedron();
	default:
		return *polyhedron;
	}
}
