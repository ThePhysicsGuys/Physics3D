#pragma once

#include "abstractMesh.h"
#include "bufferLayout.h"
#include "vertexBuffer.h"

class Mesh : public AbstractMesh {
public:
	BufferLayout bufferLayout;
	VertexBuffer* vertexBuffer = nullptr;

	const int vertexCount;

	Mesh(const double* positions, const int size);

	void render() override;
	void close() override;
};

