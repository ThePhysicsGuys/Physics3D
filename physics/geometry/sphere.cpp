#include "sphere.h"

#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/linalg/trigonometry.h"
#include "polyhedron.h"
#include "../misc/shapeLibrary.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "polyhedron.h"

Vec3f Sphere::furthestInDirection(const Vec3f& direction) const {
	return direction * (this->radius / length(direction));
}

bool Sphere::containsPoint(Vec3 point) const {
	return lengthSquared(point) < radius * radius;
}
double Sphere::getIntersectionDistance(Vec3 origin, Vec3 direction) const {
	// solve lengthSquared(origin + direction * t) == radius * radius;

	// ==> a * t^2 + 2*b*t + c = 0

	double a = lengthSquared(direction);
	double b = direction * origin;
	double c = lengthSquared(origin) - radius * radius;

	double d = sqrt(b*b - a * c);

	return (-b - d) / a;
}
double Sphere::getVolume() const {
	return M_PI * pow(radius, 3) * 4 / 3;
}

BoundingBox Sphere::getBounds() const {
	return BoundingBox(-radius, -radius, -radius, radius, radius, radius);
}
BoundingBox Sphere::getBounds(const Mat3& referenceFrame) const {
	return BoundingBox(-radius, -radius, -radius, radius, radius, radius);
}
Vec3 Sphere::getCenterOfMass() const {
	return Vec3(0, 0, 0);
}
DiagonalMat3 Sphere::getInertia() const {
	double I = M_PI * pow(radius, 5) * 8/15;
	return DiagonalMat3{I, I, I};
}
double Sphere::getMaxRadius() const {
	return radius;
}
double Sphere::getMaxRadiusSq() const {
	return radius * radius;
}

Polyhedron Sphere::asPolyhedron() const {
	return Library::createSphere(float(radius), 4);
}
