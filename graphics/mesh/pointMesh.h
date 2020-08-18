#pragma once

#include "abstractMesh.h"

namespace P3D::Graphics {

class VertexBuffer;

class PointMesh : public AbstractMesh {
public:
	VertexBuffer* vertexBuffer = nullptr;
	BufferLayout vertexBufferLayout;

	size_t vertexCount;
	size_t capacity;

	PointMesh(const float* vertices, const size_t size) : PointMesh(vertices, size, size) {};
	PointMesh(const float* vertices, const size_t size, size_t capacity);

	void update(const float* vertices, const size_t vertexCount);

	void render() override;
	void close() override;
};

};