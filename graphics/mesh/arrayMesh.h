#pragma once

#include "abstractMesh.h"

#include "../buffers/bufferLayout.h"

namespace Graphics {

class VertexBuffer;

class ArrayMesh : public AbstractMesh {
public:
	BufferLayout vertexBufferLayout;
	BufferLayout uvBufferLayout;

	VertexBuffer* vertexBuffer = nullptr;
	VertexBuffer* uvBuffer = nullptr;

	const int vertexCount;

	ArrayMesh(const float* positions, const float* uv, const unsigned int vertexCount, const unsigned int dimensions);
	ArrayMesh(const float* positions, const unsigned int vertexCount, const unsigned int dimensions, unsigned int renderMode);
	ArrayMesh(const float* positions, const unsigned int vertexCount, const unsigned int dimensions);
	ArrayMesh(Vec3f* positions, const unsigned int vertexCount);
	ArrayMesh(Vec2f* positions, const unsigned int vertexCount);

	void render() override;
	void close() override;
};

};