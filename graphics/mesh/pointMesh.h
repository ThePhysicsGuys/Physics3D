#pragma once

#include "abstractMesh.h"

class VertexBuffer;

class PointMesh : public AbstractMesh {
public:
	VertexBuffer* vertexBuffer = nullptr;
	size_t vertexCount;
	size_t capacity;

	PointMesh(const float* vertices, const size_t size) : PointMesh(vertices, size, size) {};
	PointMesh(const float* vertices, const size_t size, size_t capacity);

	void update(const float* vertices, const size_t vertexCount);

	void render() override;
	void close() override;
};
