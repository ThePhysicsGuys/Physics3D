#include "sphere.h"

#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/linalg/trigonometry.h"

#include "polyhedron.h"

Vec3f Sphere::furthestInDirection(const Vec3f& direction) const {
	return direction * (this->radius / length(direction));
}

bool Sphere::containsPoint(const Vec3f& point) const {

}
float Sphere::getIntersectionDistance(const Vec3f& origin, const Vec3f& direction) const {
	
}
double Sphere::getVolume() const {
	
}

BoundingBox Sphere::getBounds() const{}
BoundingBox Sphere::getBounds(const Mat3& referenceFrame) const{}
Vec3 Sphere::getCenterOfMass() const{}
SymmetricMat3 Sphere::getInertia() const{}
double Sphere::getMaxRadius() const{}
double Sphere::getMaxRadiusSq() const{}

Vec3f Sphere::furthestInDirection(const Vec3f& direction) const {

}

Polyhedron Sphere::asPolyhedron() const {

}