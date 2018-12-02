#pragma once

#include "abstractMesh.h"
#include "indexBuffer.h"
#include "vertexBuffer.h"
#include "bufferLayout.h"
#include "../engine/geometry/shape.h"

class IndexedMesh : public AbstractMesh {
public:
	BufferLayout bufferLayout;
	IndexBuffer* indexBuffer = nullptr;
	VertexBuffer* vertexBuffer = nullptr;

	const int vertexCount;
	const int triangleCount;

	IndexedMesh(Shape shape);
	IndexedMesh(const double* vertices, const unsigned int* indices, const int vCount, const int tCount);

	void render() override;
	void close() override;
};

