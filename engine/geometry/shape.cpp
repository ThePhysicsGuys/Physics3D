#include "shape.h"

#include <stdlib.h>
#include <cstring>
#include <math.h>

#include "../../util/Log.h"

bool Triangle::sharesEdgeWith(Triangle other) const {
	return firstIndex == other.secondIndex && secondIndex == other.firstIndex ||
		firstIndex == other.thirdIndex && secondIndex == other.secondIndex ||
		firstIndex == other.firstIndex && secondIndex == other.thirdIndex || 

		secondIndex == other.secondIndex && thirdIndex == other.firstIndex ||
		secondIndex == other.thirdIndex && thirdIndex == other.secondIndex ||
		secondIndex == other.firstIndex && thirdIndex == other.thirdIndex ||

		thirdIndex == other.secondIndex && firstIndex == other.firstIndex ||
		thirdIndex == other.thirdIndex && firstIndex == other.secondIndex ||
		thirdIndex == other.firstIndex && firstIndex == other.thirdIndex;
}

Triangle Triangle::operator~() const {
	return Triangle { firstIndex, thirdIndex, secondIndex };
}

bool Triangle::operator==(const Triangle & other) const {
	return firstIndex == other.firstIndex && secondIndex == other.secondIndex && thirdIndex == other.thirdIndex || 
		firstIndex == other.secondIndex && secondIndex == other.thirdIndex && thirdIndex == other.firstIndex ||
		firstIndex == other.thirdIndex && secondIndex == other.firstIndex && thirdIndex == other.secondIndex;
}

Triangle Triangle::rightShift() const { 
	return Triangle { thirdIndex, firstIndex, secondIndex };
}

Triangle Triangle::leftShift() const {
	return Triangle { secondIndex, thirdIndex, firstIndex };
}

Shape::Shape() : vertices(nullptr), triangles(nullptr), vCount(0), tCount(0) {}
Shape::Shape(Vec3* vertices, const Triangle* triangles, int vCount, int tCount) : vertices(vertices), normals(getNormals()), triangles(triangles), vCount(vCount), tCount(tCount) {}
Shape::Shape(Vec3* vertices, Vec3* normals, const Triangle* triangles, int vCount, int tCount) : vertices(vertices), normals(normals), triangles(triangles), vCount(vCount), tCount(tCount) {}

NormalizedShape Shape::normalized(Vec3* vecBuf, CFrame& backTransformation) const {
	backTransformation = getInertialEigenVectors();

	for (int i = 0; i < vCount; i++) {
		vecBuf[i] = backTransformation.globalToLocal(vertices[i]);
	}
	
	return NormalizedShape(vecBuf, triangles, vCount, tCount);
}

CFrame Shape::getInertialEigenVectors() const {
	Vec3 centerOfMass = getCenterOfMass();
	Mat3 inertia = getInertia(centerOfMass);
	Mat3 basis = inertia.getEigenDecomposition().eigenVectors;

	return CFrame(centerOfMass, basis);
}

NormalizedShape::NormalizedShape(Vec3* vertices, const Triangle* triangles, int vertexCount, int triangleCount) : Shape(vertices, triangles, vertexCount, triangleCount) {};

/*
	Creates a normalized shape

	Modifies `vertices`
*/
NormalizedShape::NormalizedShape(Vec3* vertices, const Triangle* triangles, int vCount, int tCount, CFrame& transformation) : Shape(vertices, triangles, vCount, tCount) {
	this->normalized(vertices, transformation);
}

Shape Shape::translated(Vec3 offset, Vec3* newVecBuf) const {
	for (int i = 0; i < this->vCount; i++) {
		newVecBuf[i] = offset + vertices[i];
	}

	return Shape(newVecBuf, triangles, vCount, tCount);
}

Shape Shape::rotated(RotMat3 rotation, Vec3* newVecBuf) const {
	for (int i = 0; i < this->vCount; i++) {
		newVecBuf[i] = rotation * vertices[i];
	}

	return Shape(newVecBuf, triangles, vCount, tCount);
}

Shape Shape::localToGlobal(CFrame frame, Vec3* newVecBuf) const {
	for (int i = 0; i < this->vCount; i++) {
		newVecBuf[i] = frame.localToGlobal(vertices[i]);
	}

	return Shape(newVecBuf, triangles, vCount, tCount);
}

Shape Shape::globalToLocal(CFrame frame, Vec3* newVecBuf) const {
	for (int i = 0; i < this->vCount; i++) {
		newVecBuf[i] = frame.globalToLocal(vertices[i]);
	}

	return Shape(newVecBuf, triangles, vCount, tCount);
}

BoundingBox Shape::getBounds() const {
	double xmin = vertices[0].x, xmax = vertices[0].x;
	double ymin = vertices[0].y, ymax = vertices[0].y;
	double zmin = vertices[0].z, zmax = vertices[0].z;

	for (int i = 1; i < vCount; i++) {
		const Vec3 current = vertices[i];

		if (current.x < xmin) xmin = current.x;
		if (current.x > xmax) xmax = current.x;
		if (current.y < ymin) ymin = current.y;
		if (current.y > ymax) ymax = current.y;
		if (current.z < zmin) zmin = current.z;
		if (current.z > zmax) zmax = current.z;
	}

	return BoundingBox { xmin, ymin, zmin, xmax, ymax, zmax };
}

// for every edge, of every triangle, check that it coincides with exactly one other triangle, in reverse order
bool isComplete(const Triangle* triangles, int tCount) {
	for (int i = 0; i < tCount; i++) {
		Triangle a = triangles[i];
		
		for (int j = 0; j < tCount; j++) {
			if (j == i) continue;

			Triangle b = triangles[j];

			if (a.sharesEdgeWith(b)) {  // correctly oriented
				goto endOfLoop;
			} else if (a.sharesEdgeWith(~b)) {	// wrongly oriented
				Log::warn("triangles[%d](%d, %d, %d) and triangles[%d](%d, %d, %d) are joined wrongly", i, a.firstIndex, a.secondIndex, a.thirdIndex, j, b.firstIndex, b.secondIndex, b.thirdIndex);
				return false;
			}
		}
		Log::warn("No triangle found that shares an edge with triangles[%d]", i);
		return false;
		endOfLoop:;
	}
	return true;
}

bool Shape::isValid() const {
	return isComplete(triangles, tCount);
}

Vec3 Shape::getNormalVecOfTriangle(Triangle triangle) const {
	Vec3 v0 = vertices[triangle.firstIndex];
	return (vertices[triangle.secondIndex] - v0) % (vertices[triangle.thirdIndex] - v0);
}

Vec3* Shape::getNormals() const {
	return nullptr;
}

/*
Checks that for every triangle, the outward-facing normal vector *dot* the vector from any of the points to the given point is negative
If at least one of these values is positive, then the point must be on the outside of that triangle, and hence, outside of the shape
only for convex shapes
*/
bool Shape::containsPoint(Vec3 point) const {
	for (int i = 0; i < tCount; i++) {
		Triangle triangle = triangles[i];
		Vec3 normalVec = getNormalVecOfTriangle(triangle);
		if((point - vertices[triangle.firstIndex]) * normalVec > 0) return false;
	}
	return true;
}

double Shape::getVolume() const {
	double total = 0;
	for (Triangle triangle : iterTriangles()) {
		Vec3 v0 = vertices[triangle.firstIndex]; 
		Vec3 v1 = vertices[triangle.secondIndex];
		Vec3 v2 = vertices[triangle.thirdIndex];

		Vec3 D1 = v1 - v0; 
		Vec3 D2 = v2 - v0;
		
		double Tf = (D1.x * D2.y - D1.y * D2.x);

		total += Tf * ((D1.z + D2.z) / 6 + v0.z / 2);
	}

	return total;
}

Vec3 Shape::getCenterOfMass() const {
	Vec3 total = Vec3(0,0,0);
	for (Triangle triangle : iterTriangles()) {
		Vec3 v0 = vertices[triangle.firstIndex];
		Vec3 v1 = vertices[triangle.secondIndex];
		Vec3 v2 = vertices[triangle.thirdIndex];

		Vec3 D1 = v1 - v0;
		Vec3 D2 = v2 - v0;

		Vec3 dFactor = D1 % D2;
		Vec3 vFactor = v0.squared() + v1.squared() + v2.squared() + v0.mul(v1) + v1.mul(v2) + v2.mul(v0);

		total += dFactor.mul(vFactor);
	}
	
	return total / (24 * getVolume());
}

Vec4 Shape::getCircumscribedSphere() const {
	BoundingBox bounds = getBounds();
	Vec3 center = Vec3(bounds.xmax - bounds.xmin, bounds.ymax - bounds.ymin, bounds.zmax - bounds.zmin) / 2.0;
	double radius = (Vec3(bounds.xmax, bounds.ymax, bounds.zmax) - center).length();
	return Vec4(center.x, center.y, center.z, radius);
}

/*
	The total inertial matrix is given by the integral over the volume of the shape of the following matrix:
	[[
	[y^2+z^2,    xy,    xz],
	[xy,    x^2+z^2,    yz],
	[xz,    yz,    x^2+y^2]
	]]

	This has been reworked to a surface integral resulting in the given formulae
*/
Mat3 Shape::getInertia(CFrame reference) const {
	Mat3 total = Mat3(0, 0, 0, 0, 0, 0, 0, 0, 0);
	for (Triangle triangle : iterTriangles()) {
		Vec3 v0 = reference.globalToLocal(vertices[triangle.firstIndex]);
		Vec3 v1 = reference.globalToLocal(vertices[triangle.secondIndex]);
		Vec3 v2 = reference.globalToLocal(vertices[triangle.thirdIndex]);

		Vec3 D1 = v1 - v0;
		Vec3 D2 = v2 - v0;

		Vec3 dFactor = D1 % D2;

		// Diagonal Elements
		Vec3 squaredIntegral = v0.cubed() + v1.cubed() + v2.cubed() + v0.squared().mul(v1 + v2) + v1.squared().mul(v0 + v2) + v2.squared().mul(v0 + v1) + v0.mul(v1).mul(v2);
		Vec3 diagonalElementParts = dFactor.mul(squaredIntegral) / 60;

		total.m00 += diagonalElementParts.y + diagonalElementParts.z;
		total.m11 += diagonalElementParts.z + diagonalElementParts.x;
		total.m22 += diagonalElementParts.x + diagonalElementParts.y;

		// Other Elements
		double selfProducts  =	v0.x*v0.y*v0.z + v1.x*v1.y*v1.z + v2.x*v2.y*v2.z;
		double twoSames      =	v0.x*v0.y*v1.z + v0.x*v1.y*v0.z + v0.x*v1.y*v1.z + v0.x*v0.y*v2.z + v0.x*v2.y*v0.z + v0.x*v2.y*v2.z +
								v1.x*v0.y*v0.z + v1.x*v1.y*v0.z + v1.x*v0.y*v1.z + v1.x*v1.y*v2.z + v1.x*v2.y*v1.z + v1.x*v2.y*v2.z +
								v2.x*v0.y*v0.z + v2.x*v1.y*v2.z + v2.x*v0.y*v2.z + v2.x*v1.y*v1.z + v2.x*v2.y*v0.z + v2.x*v2.y*v1.z;
		double allDifferents =	v0.x*v1.y*v2.z + v0.x*v2.y*v1.z + v1.x*v0.y*v2.z + v1.x*v2.y*v0.z + v2.x*v0.y*v1.z + v2.x*v1.y*v0.z;

		double xyzIntegral = -(3 * selfProducts + twoSames + allDifferents / 2) / 60;

		total.m01 += dFactor.z * xyzIntegral;
		total.m10 += dFactor.z * xyzIntegral;
		total.m02 += dFactor.y * xyzIntegral;
		total.m20 += dFactor.y * xyzIntegral;
		total.m12 += dFactor.x * xyzIntegral;
		total.m21 += dFactor.x * xyzIntegral;
	}
	
	return total;
}

Mat3 Shape::getInertia(Vec3 reference) const {
	return this->getInertia(CFrame(reference));
}

Mat3 Shape::getInertia(Mat3 reference) const {
	return this->getInertia(CFrame(reference));
}

Mat3 Shape::getInertia() const {
	return this->getInertia(CFrame());
}

double Shape::getIntersectionDistance(Vec3 origin, Vec3 direction) {
	const float EPSILON = 0.0000001f;
	double t = INFINITY;
	for (Triangle triangle : iterTriangles()) {
		Vec3 v0 = vertices[triangle.firstIndex];
		Vec3 v1 = vertices[triangle.secondIndex];
		Vec3 v2 = vertices[triangle.thirdIndex];

		Vec3 edge1, edge2, h, s, q;
		float a, f, u, v;

		edge1 = v1 - v0;
		edge2 = v2 - v0;

		h = direction % edge2;
		a = edge1 * h;

		if (a > -EPSILON && a < EPSILON) continue;   
		
		f = 1.0 / a;
		s = origin - v0;
		u = f * (s * h);

		if (u < 0.0 || u > 1.0) continue;
		
		q = s % edge1;
		v = f * direction * q;

		if (v < 0.0 || u + v > 1.0) continue;

		double r = f * edge2 * q;
		if (r > EPSILON) { 
			if (r < t) t = r;
		} else {
			//Log::debug("Line intersection but not a ray intersection");
			continue;
		}
	}

	return t;
}
