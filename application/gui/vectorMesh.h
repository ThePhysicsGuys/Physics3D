#pragma once

#include "abstractMesh.h"
#include "vertexBuffer.h"

class VectorMesh : public AbstractMesh {
public:
	VertexBuffer* vertexBuffer = nullptr;
	int vertexCount;
	int capacity;

	VectorMesh(const double* vertices, const int size) : VectorMesh(vertices, size, size) {};
	VectorMesh(const double* vertices, const int size, int capacity);

	void update(const double* vertices, const int vertexCount);

	void render() override;
	void close() override;
};
