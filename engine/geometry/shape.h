#pragma once

#include "../../util/math/vec3.h"

struct Triangle {
	unsigned int firstIndex, secondIndex, thirdIndex;
};

class Shape {
public:
	Vec3 * const vertices;
	Triangle * const triangles;
	const int vertexCount;
	const int triangleCount;
private:
	int * const copyCount;

public:
	Shape(int vertexCount, int triangleCount);

	Shape(Vec3* vertices, int vertexCount, Triangle* triangles, int triangleCount);

	~Shape();

	Shape(const Shape& s);

	
	void setVertex(int i, Vec3 vertex) { vertices[i] = vertex; };
	void setTriangle(int index, unsigned int v1, unsigned int v2, unsigned int v3) { triangles[index] = { v1, v2, v3 }; };

};
