#include "shape.h"

#include "polyhedron.h"
#include "basicShapes.h"


#define CUSTOM_DISPATCH(func) \
switch(this->type) {\
case BOX: Box(width, height, depth).func; break;\
case WEDGE: Wedge(width, height, depth).func; break;\
case SPHERE: Sphere(radius).func; break;\
case CYLINDER: Cylinder(radius, depth).func; break;\
case CONE: Cone(radius, depth).func; break;\
default: this->polyhedron->func;\
}

Shape::Shape() : polyhedron() {}
Shape::Shape(const Polyhedron* polyhedron) : polyhedron(polyhedron) {}
Shape::Shape(const Polyhedron& polyhedron) : polyhedron(new Polyhedron(polyhedron)) {}
Shape::Shape(Polyhedron&& polyhedron) : polyhedron(new Polyhedron(std::move(polyhedron))) {}

bool Shape::containsPoint(Vec3f point) const { return polyhedron->containsPoint(point); }
float Shape::getIntersectionDistance(Vec3f origin, Vec3f direction) const { return polyhedron->getIntersectionDistance(origin, direction); }
double Shape::getVolume() const { return polyhedron->getVolume(); }
Shape Shape::scaled(float scaleX, float scaleY, float scaleZ) const { return Shape(new Polyhedron(polyhedron->scaled(scaleX, scaleY, scaleZ))); }
Shape Shape::scaled(double scaleX, double scaleY, double scaleZ) const { return Shape(new Polyhedron(polyhedron->scaled(scaleX, scaleY, scaleZ))); }
BoundingBox Shape::getBounds() const { return polyhedron->getBounds(); }
BoundingBox Shape::getBounds(const Mat3& referenceFrame) const { return polyhedron->getBounds(referenceFrame); }
Vec3 Shape::getCenterOfMass() const { return polyhedron->getCenterOfMass(); }
SymmetricMat3 Shape::getInertia() const { return polyhedron->getInertia(); }
CircumscribingSphere Shape::getCircumscribingSphere() const { return polyhedron->getCircumscribingSphere(); }
double Shape::getMaxRadius() const { return polyhedron->getMaxRadius(); }

double Shape::getMaxRadiusSq() const { return polyhedron->getMaxRadiusSq(); }

Vec3f Shape::furthestInDirection(const Vec3f& direction) const { return polyhedron->furthestInDirection(direction); }

Polyhedron Shape::asPolyhedron() const { return *polyhedron; }
