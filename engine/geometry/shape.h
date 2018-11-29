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
	Shape(Vec3* vertices, int vertexCount, Triangle* triangles, int triangleCount);
	Shape(const Shape& s);

	~Shape();
};
