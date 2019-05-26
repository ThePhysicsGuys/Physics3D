#pragma once

#include "abstractMesh.h"
#include "indexBuffer.h"
#include "vertexBuffer.h"
#include "bufferLayout.h"
#include "../engine/geometry/shape.h"

class IndexedMesh : public AbstractMesh {
public:
	BufferLayout vertexBufferLayout;
	BufferLayout normalBufferLayout;
	BufferLayout uvBufferLayout;

	IndexBuffer* indexBuffer = nullptr;
	VertexBuffer* vertexBuffer = nullptr;
	VertexBuffer* normalBuffer = nullptr;
	VertexBuffer* uvBuffer = nullptr;

	const int vertexCount;
	const int triangleCount;

	IndexedMesh(const Shape&  shape);
	IndexedMesh(const float* vertices, const float* normals, const float* uvs, const unsigned int* indices, const int vertexCount, const int triangleCount);

	void render(int mode);

	void render() override;
	void close() override;
};

