#pragma once

struct ComputationBuffers;

#include "../math/vec3.h"
#include "convexShapeBuilder.h"
#include "intersection.h"

struct ComputationBuffers {
	Vec3* vertBuf;
	Triangle* triangleBuf;
	TriangleNeighbors* neighborBuf;
	EdgePiece* edgeBuf;
	int* removalBuf;
	MinkowskiPointIndices* knownVecs;

	int vertexCapacity;
	int triangleCapacity;

	ComputationBuffers(int initialVertCount, int initialTriangleCount);
	void ensureCapacity(int vertCapacity, int triangleCapacity);

	~ComputationBuffers();

	ComputationBuffers(ComputationBuffers& other) = delete;
	ComputationBuffers(ComputationBuffers&& other) = delete;
	void operator=(ComputationBuffers& other) = delete;
	void operator=(ComputationBuffers&& other) = delete;

private:
	void createVertexBuffersUnsafe(int vertexCapacity);
	void createTriangleBuffersUnsafe(int triangleCapacity);
	void deleteVertexBuffers();
	void deleteTriangleBuffers();
};