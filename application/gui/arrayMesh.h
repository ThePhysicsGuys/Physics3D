#pragma once

#include "abstractMesh.h"
#include "bufferLayout.h"
#include "vertexBuffer.h"

class ArrayMesh : public AbstractMesh {
public:
	BufferLayout bufferLayout;
	VertexBuffer* vertexBuffer = nullptr;

	const int vertexCount;

	ArrayMesh(const double* positions, const int size);

	void render() override;
	void close() override;
};

