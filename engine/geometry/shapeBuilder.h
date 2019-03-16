#pragma once

#include "shape.h"
#include "indexedShape.h"

class ShapeBuilder {
	Vec3* vertexBuf;
	Triangle* triangleBuf;
	int vertexCount;
	int triangleCount;
	TriangleNeighbors* neighborBuf;

public:
	ShapeBuilder(Vec3 * vertBuf, Triangle* triangleBuf, int vertexCount, int triangleCount, TriangleNeighbors* neighborBuf);

	void addPoint(Vec3 point, int replacingTriangleIndex);

	Shape toShape() const;
	IndexedShape toIndexedShape() const;
};
