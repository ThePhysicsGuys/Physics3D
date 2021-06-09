#include "builtinShapeClasses.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "shapeCreation.h"
#include "../misc/shapeLibrary.h"

#pragma region CubeClass
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

	return std::numeric_limits<double>::infinity();
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
#pragma endregion


#pragma region SphereClass
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
		return std::numeric_limits<double>::infinity();
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
#pragma endregion


#pragma region CylinderClass
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
				return std::numeric_limits<double>::infinity();
			}
		}
	} else {
		return std::numeric_limits<double>::infinity();
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
#pragma endregion


#pragma region WedgeClass
WedgeClass::WedgeClass() : ShapeClass(4.0, Vec3(-1 / 3, 0, 1 / 3), ScalableInertialMatrix(Vec3(0.9030, 0.6032, 0.8411), Vec3(0.3517, 0.0210, 0.1667)), WEDGE_CLASS_ID) {}

bool WedgeClass::containsPoint(Vec3 points) const {
	return std::abs(points.x) <= 1.0 && std::abs(points.y) <= 1.0 && std::abs(points.z) <= 1.0;
}
double WedgeClass::getIntersectionDistance(Vec3 origin, Vec3 direction) const {
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
	//{0, -1, 0} y-direction normal.
	//points on The plane = 0, -1, 0.
	//pointsOnThePlane = origin + t*direction.
	double distanceAlongLineFromOriginX = (1 - origin.x) / direction.x;
	Vec3 intersX = origin + distanceAlongLineFromOriginX * direction;
	if(std::abs(intersX.y) <= 1 && std::abs(intersX.z) <= 1) {
		return distanceAlongLineFromOriginX;
	}
	double distanceAlongLineFromOriginY = (1 - origin.y) / direction.y;
	Vec3 intersY = origin + distanceAlongLineFromOriginY * direction;
	if(std::abs(intersY.x) <= 1 && std::abs(intersX.z) <= 1) {
		return distanceAlongLineFromOriginY;
	}
	double distanceAlongLineFromOriginZ = (1 - origin.z) / direction.z;
	Vec3 intersZ = origin + distanceAlongLineFromOriginZ * direction;
	if(std::abs(intersX.x) <= 1 && std::abs(intersX.y) <= 1) {
		return distanceAlongLineFromOriginZ;
	}
	return std::numeric_limits<double>::infinity();
}
BoundingBox WedgeClass::getBounds(const Rotation& rotation, const DiagonalMat3& scale) const {
	const Mat3& rot = rotation.asRotationMatrix();
	Vec3 scaleRot[3] = {};
	for(size_t i = 0; i < 3; i++) {
		scaleRot[i] = scale[i] * rot.getCol(i);
	}
	const Vec3 vertices[] = {
		-scaleRot[0] - scaleRot[1] - scaleRot[2],
		-scaleRot[0] - scaleRot[1] + scaleRot[2],
		 scaleRot[0] - scaleRot[1] + scaleRot[2],
		 scaleRot[0] - scaleRot[1] - scaleRot[2],
		-scaleRot[0] + scaleRot[1] - scaleRot[2],
		-scaleRot[0] + scaleRot[1] + scaleRot[2],
	};
	double xmin = vertices[0].x; double xmax = vertices[0].x;
	double ymin = vertices[0].y; double ymax = vertices[0].y;
	double zmin = vertices[0].z; double zmax = vertices[0].z;

	for(size_t i = 1; i < 6; i++) {
		const Vec3& current = vertices[i];
		if(current.x < xmin) xmin = current.x;
		if(current.x > xmax) xmax = current.x;
		if(current.y < ymin) ymin = current.y;
		if(current.y > ymax) ymax = current.y;
		if(current.z < zmin) zmin = current.z;
		if(current.z > zmax) zmax = current.z;
	}
	return BoundingBox{xmin, ymin, zmin, xmax, ymax, zmax};
}
double WedgeClass::getScaledMaxRadiusSq(DiagonalMat3 scale) const {
	return scale[0] * scale[0] + scale[1] * scale[1] + scale[2] * scale[2];
}
Vec3f WedgeClass::furthestInDirection(const Vec3f& direction) const {
	float best = Library::wedgeVertices[0] * direction;
	Vec3f bestVertex = Library::wedgeVertices[0];

	for(size_t i = 1; i < 6; i++) {
		float current = Library::wedgeVertices[i] * direction;
		if(current > best) {
			best = current;
			bestVertex = Library::wedgeVertices[i];
		}
	}
	return bestVertex;
}
Polyhedron WedgeClass::asPolyhedron() const {
	return Library::wedge;
}
#pragma endregion

#pragma region CornerClass
CornerClass::CornerClass() : ShapeClass(4 / 3, Vec3(-2 / 3, -2 / 3, -2 / 3), ScalableInertialMatrix(Vec3(-0.0333, -0.9500, -0.0333), Vec3(0.3500, -0.0667, 0.3500)), CORNER_CLASS_ID) {}
bool CornerClass::containsPoint(Vec3 point) const {
	return std::abs(point.x) <= 1.0 && std::abs(point.y) <= 1.0 && std::abs(point.z) <= 1.0;
}
double CornerClass::getIntersectionDistance(Vec3 origin, Vec3 direction) const {
	return 2.0;
}
BoundingBox CornerClass::getBounds(const Rotation& rotation, const DiagonalMat3& scale) const {
	const Mat3& rot = rotation.asRotationMatrix();
	Vec3 scaleRot[3] = {};
	for(size_t i = 0; i < 3; i++) {
		scaleRot[i] = scale[i] * rot.getCol(i);
	}
	const Vec3 vertices[] = {
		scaleRot[0] + scaleRot[1] + scaleRot[2],
	   -scaleRot[0] + scaleRot[1] + scaleRot[2],
		scaleRot[0] - scaleRot[1] + scaleRot[2],
		scaleRot[0] + scaleRot[1] - scaleRot[2],
	};
	double xmin = vertices[0].x; double xmax = vertices[0].x;
	double ymin = vertices[0].y; double ymax = vertices[0].y;
	double zmin = vertices[0].z; double zmax = vertices[0].z;

	for(size_t i = 1; i < 4; i++) {
		const Vec3& current = vertices[i];
		if(current.x < xmin) xmin = current.x;
		if(current.x > xmax) xmax = current.x;
		if(current.y < ymin) ymin = current.y;
		if(current.y > ymax) ymax = current.y;
		if(current.z < zmin) zmin = current.z;
		if(current.z > zmax) zmax = current.z;
	}
	return BoundingBox{xmin, ymin, zmin, xmax, ymax, zmax};
}
double CornerClass::getScaledMaxRadiusSq(DiagonalMat3 scale) const {
	return scale[0] * scale[0] + scale[1] * scale[1] + scale[2] * scale[2];
}
Vec3f CornerClass::furthestInDirection(const Vec3f& direction) const {
	float bestDir = Library::cornerVertices[0] * direction;
	Vec3f bestVertex = Library::cornerVertices[0];
	for(int i = 1; i < Library::cornerVertexCount; i++) {
		float currBest = Library::cornerVertices[i] * direction;
		if(currBest > bestDir) {
			bestDir = currBest;
			bestVertex = Library::cornerVertices[i];
		}
	}
	return bestVertex;
}
Polyhedron CornerClass::asPolyhedron() const {
	return Library::corner;
}
#pragma endregion

#pragma region PolyhedronShapeClass
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
#pragma endregion

#pragma region PolyhedronShapeClassAVX
BoundingBox PolyhedronShapeClassAVX::getBounds(const Rotation& rotation, const DiagonalMat3& scale) const {
	return poly.getBoundsAVX(Mat3f(rotation.asRotationMatrix() * scale));
}
Vec3f PolyhedronShapeClassAVX::furthestInDirection(const Vec3f& direction) const {
	return poly.furthestInDirectionAVX(direction);
}

BoundingBox PolyhedronShapeClassSSE::getBounds(const Rotation& rotation, const DiagonalMat3& scale) const {
	return poly.getBoundsSSE(Mat3f(rotation.asRotationMatrix() * scale));
}
Vec3f PolyhedronShapeClassSSE::furthestInDirection(const Vec3f& direction) const {
	return poly.furthestInDirectionSSE(direction);
}

BoundingBox PolyhedronShapeClassSSE4::getBounds(const Rotation& rotation, const DiagonalMat3& scale) const {
	return poly.getBoundsSSE(Mat3f(rotation.asRotationMatrix() * scale));
}
Vec3f PolyhedronShapeClassSSE4::furthestInDirection(const Vec3f& direction) const {
	return poly.furthestInDirectionSSE4(direction);
}

BoundingBox PolyhedronShapeClassFallback::getBounds(const Rotation& rotation, const DiagonalMat3& scale) const {
	return poly.getBoundsFallback(Mat3f(rotation.asRotationMatrix() * scale));
}
Vec3f PolyhedronShapeClassFallback::furthestInDirection(const Vec3f& direction) const {
	return poly.furthestInDirectionFallback(direction);
}
#pragma endregion

const CubeClass CubeClass::instance;
const SphereClass SphereClass::instance;
const CylinderClass CylinderClass::instance;
const WedgeClass WedgeClass::instance;
const CornerClass CornerClass::instance;


