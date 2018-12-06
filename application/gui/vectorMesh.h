#pragma once

#include "abstractMesh.h"
#include "vertexBuffer.h"

class VectorMesh : public AbstractMesh {
public:
	VertexBuffer* verticesBuffer = nullptr;

	const int vertexCount;

	VectorMesh(const double* positions, const int size);

	void render() override;
	void close() override;
};
