#pragma once

#include "../math/vec3.h"
#include "../math/mat3.h"

struct Triangle {
	unsigned int firstIndex, secondIndex, thirdIndex;
};

struct Shape {
	friend class ManagedShape;
private:
	Vec3 * vertices;
	Triangle * triangles;
	int vCount;
	int tCount;

public:
	Shape();
	Shape::Shape(Vec3 * vertices, Triangle * triangles, int vertexCount, int triangleCount);

	friend Vec3* getVertPointer(Shape& s) { return s.vertices; }
	friend Triangle* getTriPointer(Shape& s) { return s.triangles; }

	const Vec3 * getVertices() { return vertices; }
	const Triangle * getTriangles() { return triangles; }
	int vertexCount() { return vCount; }
	int triangleCount() { return tCount; }

	Shape translated(Vec3 offset, Vec3* newVecBuf) const;
	Shape rotated(RotMat3 rotation, Vec3* newVecBuf) const;
};
