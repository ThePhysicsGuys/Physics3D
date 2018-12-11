#pragma once

#include "abstractMesh.h"
#include "bufferLayout.h"
#include "vertexBuffer.h"
#include "../../engine/math/vec3.h"
#include "../../engine/math/vec2.h"

class ArrayMesh : public AbstractMesh {
public:
	BufferLayout bufferLayout;
	VertexBuffer* vertexBuffer = nullptr;

	const int vertexCount;

	ArrayMesh(const double* positions, const int size, const int dimensions, RenderMode renderMode);
	ArrayMesh(const double* positions, const int size, const int dimensions) : ArrayMesh(positions, size, dimensions, RenderMode::TRIANGLES) {};
	ArrayMesh(const Vec3* positions, const int size) : ArrayMesh((double *) positions, size * 3, 3) {};
	ArrayMesh(const Vec2* positions, const int size) : ArrayMesh((double *) positions, size * 2, 2) {};

	void render() override;
	void close() override;
};

