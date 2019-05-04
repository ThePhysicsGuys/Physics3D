#include "shape.h"

#include <stdlib.h>
#include <cstring>
#include <vector>
#include <set>
#include <math.h>

#include "convexShapeBuilder.h"

#include "../math/utils.h"

#include "../../util/Log.h"

#include "../engineException.h"

#include "../debug.h"
#include "../math/mathUtil.h"

#include "computationBuffer.h"

#include "intersection.h"

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

Shape::Shape() : vertices(nullptr), 
				 triangles(nullptr), 
	 			 normals(nullptr),
				 uvs(nullptr),
				 vertexCount(0), 
				 triangleCount(0) {}

Shape::Shape(Vec3* vertices, const Triangle* triangles, int vertexCount, int triangleCount) : vertices(vertices), 
																							  triangles(triangles), 
																							  vertexCount(vertexCount), 
																							  triangleCount(triangleCount),
																							  normals(nullptr),
																							  uvs(nullptr) {}

Shape::Shape(Vec3* vertices, Vec3* normals, Vec2* uvs, const Triangle* triangles, int vertexCount, int triangleCount) :	vertices(vertices), 
																														normals(normals), 
																														uvs(uvs), 
																														triangles(triangles), 
																														vertexCount(vertexCount), 
																														triangleCount(triangleCount) {}

Shape::Shape(Vec3* vertices, Vec2* uvs, const Triangle* triangles, int vertexCount, int triangleCount) : vertices(vertices),
																										 uvs(uvs), 
																										 triangles(triangles),
																										 vertexCount(vertexCount), 
																										 triangleCount(triangleCount),
																										 normals(nullptr) {}

Shape::Shape(Vec3* vertices, Vec3* normals, const Triangle* triangles, int vertexCount, int triangleCount) : vertices(vertices), 
																											 normals(normals), 
																											 triangles(triangles), 
																											 vertexCount(vertexCount), 
																											 triangleCount(triangleCount),
																											 uvs(nullptr) {}

NormalizedShape Shape::normalized(Vec3* vecBuf, CFrame& backTransformation) const {
	backTransformation = getInertialEigenVectors();

	for (int i = 0; i < vertexCount; i++) {
		vecBuf[i] = backTransformation.globalToLocal(vertices[i]);
	}
	
	return NormalizedShape(vecBuf, normals.get(), uvs.get(), triangles, vertexCount, triangleCount);
}

CenteredShape Shape::centered(Vec3* vecBuf, Vec3& backOffset) const {
	backOffset = getCenterOfMass();

	for(int i = 0; i < vertexCount; i++) {
		vecBuf[i] = vertices[i] - backOffset;
	}

	return CenteredShape(vecBuf, triangles, vertexCount, triangleCount);
}

CFrame Shape::getInertialEigenVectors() const {
	Vec3 centerOfMass = getCenterOfMass();
	SymmetricMat3 inertia = getInertia(centerOfMass);
	Mat3 basis = inertia.getEigenDecomposition().eigenVectors;

	return CFrame(centerOfMass, basis);
}

/* Test */
NormalizedShape::NormalizedShape(Vec3 * vertices, Vec3 * normals, Vec2 * uvs, const Triangle * triangles, int vertexCount, int triangleCount) : CenteredShape(vertices, normals, uvs, triangles, vertexCount, triangleCount) {
	// TODO add normalization verification
};

NormalizedShape::NormalizedShape(Vec3 * vertices, const Triangle * triangles, int vertexCount, int triangleCount) : CenteredShape(vertices, triangles, vertexCount, triangleCount) {
	// TODO add normalization verification
};
/*
	Creates a normalized shape

	Modifies `vertices`
*/
NormalizedShape::NormalizedShape(Vec3 * vertices, const Triangle * triangles, int vertexCount, int triangleCount, CFrame& transformation) : CenteredShape(vertices, triangles, vertexCount, triangleCount) {
	this->normalized(vertices, transformation);
}


CenteredShape::CenteredShape(Vec3 * vertices, Vec3 * normals, Vec2 * uvs, const Triangle * triangles, int vertexCount, int triangleCount) : Shape(vertices, normals, uvs, triangles, vertexCount, triangleCount) {

};

CenteredShape::CenteredShape(Vec3 * vertices, const Triangle * triangles, int vertexCount, int triangleCount) : Shape(vertices, triangles, vertexCount, triangleCount) {
	// TODO add centering verification
};

CenteredShape::CenteredShape(Vec3 * vertices, const Triangle * triangles, int vertexCount, int triangleCount, Vec3& offset) : Shape(vertices, triangles, vertexCount, triangleCount) {
	this->centered(vertices, offset);
}

Shape Shape::translated(Vec3 offset, Vec3 * newVecBuf) const {
	for (int i = 0; i < this->vertexCount; i++) {
		newVecBuf[i] = offset + vertices[i];
	}

	return Shape(newVecBuf, triangles, vertexCount, triangleCount);
}

Shape Shape::rotated(RotMat3 rotation, Vec3* newVecBuf) const {
	for (int i = 0; i < this->vertexCount; i++) {
		newVecBuf[i] = rotation * vertices[i];
	}

	return Shape(newVecBuf, triangles, vertexCount, triangleCount);
}

Shape Shape::localToGlobal(CFrame frame, Vec3* newVecBuf) const {
	for (int i = 0; i < this->vertexCount; i++) {
		newVecBuf[i] = frame.localToGlobal(vertices[i]);
	}

	return Shape(newVecBuf, triangles, vertexCount, triangleCount);
}

Shape Shape::globalToLocal(CFrame frame, Vec3* newVecBuf) const {
	for (int i = 0; i < this->vertexCount; i++) {
		newVecBuf[i] = frame.globalToLocal(vertices[i]);
	}

	return Shape(newVecBuf, triangles, vertexCount, triangleCount);
}

BoundingBox Shape::getBounds() const {
	double xmin = vertices[0].x, xmax = vertices[0].x;
	double ymin = vertices[0].y, ymax = vertices[0].y;
	double zmin = vertices[0].z, zmax = vertices[0].z;

	for (int i = 1; i < vertexCount; i++) {
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
bool isComplete(const Triangle* triangles, int triangleCount) {
	for (int i = 0; i < triangleCount; i++) {
		Triangle a = triangles[i];
		
		for (int j = 0; j < triangleCount; j++) {
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
	return isComplete(triangles, triangleCount) && getVolume() >= 0;
}

Vec3 Shape::getNormalVecOfTriangle(Triangle triangle) const {
	Vec3 v0 = vertices[triangle.firstIndex];
	return (vertices[triangle.secondIndex] - v0) % (vertices[triangle.thirdIndex] - v0);
}

void Shape::computeNormals(Vec3* buffer) const {
	for (int i = 0; i < vertexCount; i++) {
		Vec3 vertex = vertices[i];
		Vec3 vertexNormal;
		for (int j = 0; j < triangleCount; j++) {
			Triangle triangle = triangles[j];
			if (triangle.firstIndex == i || triangle.secondIndex == i || triangle.thirdIndex == i) {

				while (triangle.firstIndex != i)
					triangle = triangle.rightShift();

				Vec3 v0 = vertices[triangle.firstIndex];
				Vec3 v1 = vertices[triangle.secondIndex];
				Vec3 v2 = vertices[triangle.thirdIndex];

				Vec3 D1 = v1 - v0;
				Vec3 D2 = v2 - v0;

				Vec3 faceNormal = D1 % D2;

				double sin = faceNormal.length() / (D1.length() * D2.length());
				vertexNormal += faceNormal.normalize() * asin(sin);
			}
		}
		vertexNormal = vertexNormal.normalize();
		buffer[i] = vertexNormal;
	}
}

bool Shape::containsPoint(Vec3 point) const {
	Vec3 ray = Vec3(1, 0, 0);

	bool isExiting = false;
	double bestD = static_cast<double>(INFINITY);

	for(const Triangle& tri : iterTriangles()) {
		RayIntersection r = rayTriangleIntersection(point, ray, vertices[tri.firstIndex], vertices[tri.secondIndex], vertices[tri.thirdIndex]);
		if(r.d >= 0 && r.lineIntersectsTriangle()) {
			if(r.d < bestD) {
				bestD = r.d;
				isExiting = (getNormalVecOfTriangle(tri) * ray >= 0);
			}
		}
	}

	return isExiting;
}

int Shape::furthestIndexInDirection(Vec3 direction) const {
	double bestDot = vertices[0] * direction;
	int bestVertexIndex = 0;
	for(int i = 1; i < vertexCount; i++) {
		double newD = vertices[i] * direction;
		if(newD > bestDot) {
			bestDot = newD;
			bestVertexIndex = i;
		}
	}

	return bestVertexIndex;
}

Vec3 Shape::furthestInDirection(Vec3 direction) const {
	double bestDot = vertices[0] * direction;
	Vec3 bestVertex = vertices[0];
	for(int i = 1; i < vertexCount; i++) {
		double newD = vertices[i] * direction;
		if(newD > bestDot) {
			bestDot = newD;
			bestVertex = vertices[i];
		}
	}

	return bestVertex;
}

ComputationBuffers buffers(1000, 2000);
bool Shape::intersects(const Shape& other, Vec3& intersection, Vec3& exitVector, const Vec3& centerConnection) const {
	Tetrahedron result;
	bool collides = runGJK(*this, other, centerConnection, result);
	
	if(collides) {
		bool epaResult = runEPA(*this, other, result, intersection, exitVector, buffers);
		return epaResult;
	} else {
		return false;
	}
}

#include "../physicsProfiler.h"

bool Shape::intersectsTransformed(const Shape& other, const CFrame& relativeCFrame, Vec3& intersection, Vec3& exitVector) const {
	Tetrahedron result;
	bool collides = runGJKTransformed(*this, other, relativeCFrame, relativeCFrame.position, result);
	
	if(collides) {
		return runEPATransformed(*this, other, result, relativeCFrame, intersection, exitVector, buffers);
	} else {
		return false;
	}
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

Sphere Shape::getCircumscribedSphere() const {
	BoundingBox bounds = getBounds();
	Vec3 center = Vec3(bounds.xmax + bounds.xmin, bounds.ymax + bounds.ymin, bounds.zmax + bounds.zmin) / 2.0;
	double radius = (Vec3(bounds.xmax, bounds.ymax, bounds.zmax) - center).length();
	return Sphere{center, radius};
}

double Shape::getMaxRadius() const {
	double bestDistSq = 0;
	for(Vec3 vertex : iterVertices()) {
		double distSq = vertex.lengthSquared();
		if(distSq > bestDistSq) {
			bestDistSq = distSq;
		}
	}
	return sqrt(bestDistSq);
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
SymmetricMat3 Shape::getInertia(CFrame reference) const {
	SymmetricMat3 total(0, 0, 0, 0, 0, 0);
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
		total.m02 += dFactor.y * xyzIntegral;
		total.m12 += dFactor.x * xyzIntegral;
	}
	
	return total;
}

SymmetricMat3 Shape::getInertia(Vec3 reference) const {
	return this->getInertia(CFrame(reference));
}

SymmetricMat3 Shape::getInertia(Mat3 reference) const {
	return this->getInertia(CFrame(reference));
}

SymmetricMat3 Shape::getInertia() const {
	return this->getInertia(CFrame());
}

double Shape::getIntersectionDistance(Vec3 origin, Vec3 direction) {
	const double EPSILON = 0.0000001f;
	double t = INFINITY;
	for (Triangle triangle : iterTriangles()) {
		Vec3 v0 = vertices[triangle.firstIndex];
		Vec3 v1 = vertices[triangle.secondIndex];
		Vec3 v2 = vertices[triangle.thirdIndex];

		Vec3 edge1, edge2, h, s, q;
		double a, f, u, v;

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
