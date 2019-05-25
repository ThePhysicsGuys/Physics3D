#pragma once

#include "abstractMesh.h"
#include "vertexBuffer.h"

class VectorMesh : public AbstractMesh {
public:
	VertexBuffer* vertexBuffer = nullptr;
	size_t vertexCount;
	size_t capacity;

	VectorMesh(const double* vertices, const size_t size) : VectorMesh(vertices, size, size) {};
	VectorMesh(const double* vertices, const size_t size, size_t capacity);

	void update(const double* vertices, const size_t vertexCount);

	void render() override;
	void close() override;
};
