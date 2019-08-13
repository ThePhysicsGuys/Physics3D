#pragma once

#include "abstractMesh.h"

#include "../buffers/bufferLayout.h"

#include "../engine/math/vec.h"

class VertexBuffer;

class ArrayMesh : public AbstractMesh {
public:
	BufferLayout vertexBufferLayout;
	BufferLayout uvBufferLayout;

	VertexBuffer* vertexBuffer = nullptr;
	VertexBuffer* uvBuffer = nullptr;

	const int vertexCount;

	ArrayMesh(const float* positions, const float* uv, const unsigned int vertexCount, const unsigned int dimensions);
	ArrayMesh(const float* positions, const unsigned int vertexCount, const unsigned int dimensions, int renderMode);
	ArrayMesh(const float* positions, const unsigned int vertexCount, const unsigned int dimensions) : ArrayMesh(positions, vertexCount, dimensions, RenderMode::TRIANGLES) {};
	ArrayMesh(Vec3f* positions, const unsigned int vertexCount) : ArrayMesh(reinterpret_cast<float const *>(positions), vertexCount * 3, 3) {};
	ArrayMesh(Vec2f* positions, const unsigned int vertexCount) : ArrayMesh(reinterpret_cast<float const *>(positions), vertexCount * 2, 2) {};

	void render() override;
	void close() override;
};

