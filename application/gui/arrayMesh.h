#pragma once

#include "abstractMesh.h"
#include "bufferLayout.h"
#include "vertexBuffer.h"
#include "../../engine/math/vec3.h"
#include "../../engine/math/vec2.h"

class ArrayMesh : public AbstractMesh {
public:
	BufferLayout vertexBufferLayout;
	BufferLayout uvBufferLayout;

	VertexBuffer* vertexBuffer = nullptr;
	VertexBuffer* uvBuffer = nullptr;

	const int vertexCount;

	ArrayMesh(const double* positions, const double* uv, const unsigned int vertexCount, const unsigned int dimensions);
	ArrayMesh(const double* positions, const unsigned int vertexCount, const unsigned int dimensions, RenderMode renderMode);
	ArrayMesh(const double* positions, const unsigned int vertexCount, const unsigned int dimensions) : ArrayMesh(positions, vertexCount, dimensions, RenderMode::TRIANGLES) {};
	ArrayMesh(const Vec3* positions, const unsigned int vertexCount) : ArrayMesh((double *) positions, vertexCount * 3, 3) {};
	ArrayMesh(const Vec2* positions, const unsigned int vertexCount) : ArrayMesh((double *) positions, vertexCount * 2, 2) {};

	void render() override;
	void close() override;
};

