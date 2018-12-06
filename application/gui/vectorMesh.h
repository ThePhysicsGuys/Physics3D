#pragma once

#include "abstractMesh.h"
#include "vertexBuffer.h"

class VectorMesh : public AbstractMesh {
public:
	VertexBuffer* positionBuffer = nullptr;
	VertexBuffer* rotationBuffer = nullptr;

	const int vertexCount;

	VectorMesh(const double* positions, const double* rotations, const int size);

	void render() override;
	void close() override;
};
