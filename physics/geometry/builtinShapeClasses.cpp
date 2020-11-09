#include "builtinShapeClasses.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "shapeCreation.h"
#include "../misc/shapeLibrary.h"


CubeClass::CubeClass() : ShapeClass(8, Vec3(0, 0, 0), ScalableInertialMatrix(Vec3(8.0 / 3.0, 8.0 / 3.0, 8.0 / 3.0), Vec3(0, 0, 0)), CUBE_CLASS_ID) {}

bool CubeClass::containsPoint(Vec3 point) const {
	return std::abs(point.x) <= 1.0 && std::abs(point.y) <= 1.0 && std::abs(point.z) <= 1.0;
}

double CubeClass::getIntersectionDistance(Vec3 origin, Vec3 direction) const {
	if(origin.x < 0) {
		origin.x = -origin.x;
		direction.x = -direction.x;
	}
	if(origin.y < 0) {
		origin.y = -origin.y;
		direction.y = -direction.y;
	}
	if(origin.z < 0) {
		origin.z = -origin.z;
		direction.z = -direction.z;
	}

	//origin + t * direction = x1z
	double tx = (1 - origin.x) / direction.x;

	Vec3 intersX = origin + tx * direction;
	if(std::abs(intersX.y) <= 1.0 && std::abs(intersX.z) <= 1.0) return tx;

	double ty = (1 - origin.y) / direction.y;

	Vec3 intersY = origin + ty * direction;
	if(std::abs(intersY.x) <= 1.0 && std::abs(intersY.z) <= 1.0) return ty;

	double tz = (1 - origin.z) / direction.z;

	Vec3 intersZ = origin + tz * direction;
	if(std::abs(intersZ.x) <= 1.0 && std::abs(intersZ.y) <= 1.0) return tz;

	return INFINITY;
}

BoundingBox CubeClass::getBounds(const Rotation& rotation, const DiagonalMat3& scale) const {
	Mat3 referenceFrame = rotation.asRotationMatrix() * scale;
	double x = std::abs(referenceFrame(0, 0)) + std::abs(referenceFrame(0, 1)) + std::abs(referenceFrame(0, 2));
	double y = std::abs(referenceFrame(1, 0)) + std::abs(referenceFrame(1, 1)) + std::abs(referenceFrame(1, 2));
	double z = std::abs(referenceFrame(2, 0)) + std::abs(referenceFrame(2, 1)) + std::abs(referenceFrame(2, 2));
	BoundingBox result{-x,-y,-z,x,y,z};
	return result;
}

double CubeClass::getScaledMaxRadiusSq(DiagonalMat3 scale) const {
	return scale[0] * scale[0] + scale[1] * scale[1] + scale[2] * scale[2];
}

Vec3f CubeClass::furthestInDirection(const Vec3f& direction) const {
	return Vec3f(direction.x < 0 ? -1.0f : 1.0f, direction.y < 0 ? -1.0f : 1.0f, direction.z < 0 ? -1.0f : 1.0f);
}

Polyhedron CubeClass::asPolyhedron() const {
	return Library::createCube(2.0);
}



SphereClass::SphereClass() : ShapeClass(4.0 / 3.0 * M_PI, Vec3(0, 0, 0), ScalableInertialMatrix(Vec3(4.0 / 15.0 * M_PI, 4.0 / 15.0 * M_PI, 4.0 / 15.0 * M_PI), Vec3(0, 0, 0)), SPHERE_CLASS_ID) {}

bool SphereClass::containsPoint(Vec3 point) const {
	return lengthSquared(point) <= 1.0;
}

double SphereClass::getIntersectionDistance(Vec3 origin, Vec3 direction) const {
	//o*o + o*d*t + o*d*t + t*t*d*d
	double c = origin * origin - 1;
	double b = origin * direction;
	double a = direction * direction;

	// solve a*t^2 + 2*b*t + c
	double D = b * b - a * c;
	if(D >= 0) {
		return (-b + -sqrt(D)) / a;
	} else {
		return INFINITY;
	}
}

BoundingBox SphereClass::getBounds(const Rotation& rotation, const DiagonalMat3& scale) const {
	double s = scale[0];
	return BoundingBox{-s, -s, -s, s, s, s};
}

double SphereClass::getScaledMaxRadiusSq(DiagonalMat3 scale) const {
	return scale[0] * scale[0];
}

double SphereClass::getScaledMaxRadius(DiagonalMat3 scale) const {
	return scale[0];
}

Vec3f SphereClass::furthestInDirection(const Vec3f& direction) const {
	float lenSq = lengthSquared(direction);
	if(lenSq == 0.0f) return Vec3f(1.0f, 0.0f, 0.0f);
	return direction / std::sqrt(lenSq);
}

Polyhedron SphereClass::asPolyhedron() const {
	return Library::createSphere(1.0, 3);
}

void SphereClass::setScaleX(double newX, DiagonalMat3& scale) const {
	scale[0] = newX;
	scale[1] = newX;
	scale[2] = newX;
}

void SphereClass::setScaleY(double newY, DiagonalMat3& scale) const {
	scale[0] = newY;
	scale[1] = newY;
	scale[2] = newY;
}

void SphereClass::setScaleZ(double newZ, DiagonalMat3& scale) const {
	scale[0] = newZ;
	scale[1] = newZ;
	scale[2] = newZ;
}



/*
Inertia of cyllinder:
	z = V * 1/2
	x, y = V * 7/12

	X = y + z = 7/12
	Y = x + z = 7/12
	Z = x + y = 1/2

	x = 1/4 * M_PI * 2
	y = 1/4 * M_PI * 2
	z = 1/3 * M_PI * 2
*/
CylinderClass::CylinderClass() : ShapeClass(M_PI * 2.0, Vec3(0, 0, 0), ScalableInertialMatrix(Vec3(M_PI / 2.0, M_PI / 2.0, M_PI * 2.0 / 3.0), Vec3(0, 0, 0)), CYLINDER_CLASS_ID) {}

bool CylinderClass::containsPoint(Vec3 point) const {
	return std::abs(point.z) <= 1.0 && point.x * point.x + point.y + point.y <= 1.0;
}

double CylinderClass::getIntersectionDistance(Vec3 origin, Vec3 direction) const {
	Vec2 xyOrigin(origin.x, origin.y);
	Vec2 xyDirection(direction.x, direction.y);

	//o*o + o*d*t + o*d*t + t*t*d*d
	double c = xyOrigin * xyOrigin - 1;
	double b = xyOrigin * xyDirection;
	double a = xyDirection * xyDirection;

	// solve a*t^2 + 2*b*t + c
	double D = b * b - a * c;
	if(D >= 0) {
		double t = (-b + -sqrt(D)) / a;
		double z = origin.z + t * direction.z;
		if(std::abs(z) <= 1.0) {
			return t;
		} else {
			// origin + t * direction = 1 => t = (1-origin)/direction

			double t2 = (((origin.z >= 0) ? 1 : -1) - origin.z) / direction.z;

			double x = origin.x + t2 * direction.x;
			double y = origin.y + t2 * direction.y;

			if(x * x + y * y <= 1.0) {
				return t2;
			} else {
				return INFINITY;
			}
		}
	} else {
		return INFINITY;
	}
}

BoundingBox CylinderClass::getBounds(const Rotation& rotation, const DiagonalMat3& scale) const {
	double height = scale[2];
	double radius = scale[0];

	Vec3 normalizedZVector = rotation.getZ();
	Vec3 zVector = normalizedZVector * height;

	double extraX = std::hypot(normalizedZVector.y, normalizedZVector.z);
	double extraY = std::hypot(normalizedZVector.x, normalizedZVector.z);
	double extraZ = std::hypot(normalizedZVector.x, normalizedZVector.y);

	double x = std::abs(zVector.x) + extraX * radius;
	double y = std::abs(zVector.y) + extraY * radius;
	double z = std::abs(zVector.z) + extraZ * radius;

	return BoundingBox{-x, -y, -z, x, y, z};
}

double CylinderClass::getScaledMaxRadiusSq(DiagonalMat3 scale) const {
	return scale[0] * scale[0] + scale[2] * scale[2];
}

Vec3f CylinderClass::furthestInDirection(const Vec3f& direction) const {
	float z = (direction.z >= 0.0f) ? 1.0f : -1.0f;
	float lenSq = direction.x * direction.x + direction.y * direction.y;
	if(lenSq == 0.0) return Vec3f(1.0f, 0.0f, z);
	float length = sqrt(lenSq);
	return Vec3f(direction.x / length, direction.y / length, z);
}

Polyhedron CylinderClass::asPolyhedron() const {
	return Library::createPrism(64, 1.0, 2.0);
}

void CylinderClass::setScaleX(double newX, DiagonalMat3& scale) const {
	scale[0] = newX;
	scale[1] = newX;
}

void CylinderClass::setScaleY(double newY, DiagonalMat3& scale) const {
	scale[0] = newY;
	scale[1] = newY;
}

PolyhedronShapeClass::PolyhedronShapeClass(Polyhedron&& poly) : poly(poly), ShapeClass(poly.getVolume(), poly.getCenterOfMass(), poly.getScalableInertiaAroundCenterOfMass(), CONVEX_POLYHEDRON_CLASS_ID) {}

bool PolyhedronShapeClass::containsPoint(Vec3 point) const {
	return poly.containsPoint(point);
}
double PolyhedronShapeClass::getIntersectionDistance(Vec3 origin, Vec3 direction) const {
	return poly.getIntersectionDistance(origin, direction);
}
BoundingBox PolyhedronShapeClass::getBounds(const Rotation& rotation, const DiagonalMat3& scale) const {
	return poly.getBounds(Mat3f(rotation.asRotationMatrix() * scale));
}
double PolyhedronShapeClass::getScaledMaxRadius(DiagonalMat3 scale) const {
	return poly.getScaledMaxRadius(scale);
}
double PolyhedronShapeClass::getScaledMaxRadiusSq(DiagonalMat3 scale) const {
	return poly.getScaledMaxRadiusSq(scale);
}
Vec3f PolyhedronShapeClass::furthestInDirection(const Vec3f& direction) const {
	return poly.furthestInDirection(direction);
}
Polyhedron PolyhedronShapeClass::asPolyhedron() const {
	return poly;
}


const CubeClass CubeClass::instance;
const SphereClass SphereClass::instance;
const CylinderClass CylinderClass::instance;

