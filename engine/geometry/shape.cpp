#include "shape.h"

#include <stdlib.h>
#include <cstring>


Shape::Shape(Vec3 * vertices, int vertexCount, Triangle * triangles, int triangleCount) : vertexCount(vertexCount), triangleCount(triangleCount), copyCount(new int(0)), 
		vertices(new Vec3[vertexCount]), triangles(new Triangle[triangleCount]) {
	std::memcpy(this->vertices, vertices, vertexCount * sizeof(Vec3));
	std::memcpy(this->triangles, triangles, triangleCount * sizeof(Triangle));
}

Shape::Shape(const Shape & s) : vertexCount(s.vertexCount), triangleCount(triangleCount), copyCount(s.copyCount), vertices(s.vertices), triangles(s.triangles) {
	(*copyCount)++;
}

Shape::~Shape() {
	if (*copyCount == 0) {
		delete[] vertices;
		delete[] triangles;
		delete copyCount;
	} else {
		(*copyCount)--;
	}
}
