#pragma once

struct Triangle;
struct Shape;

#include "../math/vec3.h"
#include "../math/vec4.h"
#include "../math/mat3.h"
#include "../math/cframe.h"
#include "boundingBox.h"

struct Triangle {
	unsigned int firstIndex, secondIndex, thirdIndex;

	bool sharesEdgeWith(Triangle other) const;
	Triangle operator~() const;
	bool operator==(const Triangle& other) const;
	Triangle rightShift() const;
	Triangle leftShift() const;
};

struct NormalizedShape;

struct Shape {
	struct TriangleIter {
		Triangle* first;
		int size;
		inline Triangle* begin() const { return first; };
		inline Triangle* end() const { return first + size; };
	};
	struct VertexIter {
		Vec3* first;
		int size;
		inline Vec3* begin() const { return first; };
		inline Vec3* end() const { return first + size; };
	};

	Vec3 * vertices;
	Triangle * triangles;
	int vCount;
	int tCount;

	Shape();
	Shape(Vec3 * vertices, Triangle * triangles, int vertexCount, int triangleCount);

	Shape translated(Vec3 offset, Vec3* newVecBuf) const;
	Shape rotated(RotMat3 rotation, Vec3* newVecBuf) const;
	Shape localToGlobal(CFrame frame, Vec3* newVecBuf) const;
	Shape globalToLocal(CFrame frame, Vec3* newVecBuf) const;

	CFrame normalize();

	CFrame getInertialNormalAxes() const;

	BoundingBox getBounds() const;

	double getIntersectionDistance(Vec3 origin, Vec3 direction);

	double getVolume() const;
	Vec3 getCenterOfMass() const;
	Mat3 getInertia() const;
	Mat3 getInertia(Vec3 reference) const;
	Mat3 getInertia(Mat3 reference) const;
	Mat3 getInertia(CFrame reference) const;

	Vec4 getCircumscribedSphere() const;

	bool containsPoint(Vec3 point) const;
	Vec3 getNormalVecOfTriangle(Triangle t) const;

	inline TriangleIter iterTriangles() const { return TriangleIter{triangles, tCount}; };
	inline VertexIter iterVertices() const { return VertexIter{vertices, vCount}; };

	bool isValid() const;
};

struct NormalizedShape : public Shape {
	friend struct Shape;
private:
	NormalizedShape(const Vec3 * vertices, const Triangle * triangles, int vertexCount, int triangleCount);
};
