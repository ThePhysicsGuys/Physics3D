#pragma once

#include "abstractMesh.h"
#include "../engine/geometry/shape.h"

class IndexedMesh : public AbstractMesh {
public:
	unsigned int vao;
	unsigned int posVbo;
	unsigned int indVbo;

	const int vertexCount;
	const int triangleCount;

	IndexedMesh(Shape shape);
	IndexedMesh(double* vertices, int vertexCount, unsigned int* triangles, int triangleCount);
	IndexedMesh() : vertexCount(0), triangleCount(0) {};

	void render() override;
	void close() override;
};

