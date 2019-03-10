#pragma once

#include "shape.h"
#include "indexedShape.h"

struct ShapeBuilder {
	Vec3* vertexBuf;
	Triangle* triangleBuf;
	int vertexCount;
	int triangleCount;
	TriangleNeighbors* neighborBuf;

	ShapeBuilder(Vec3 * vertBuf, Triangle* triangleBuf, int vertexCount, int triangleCount, TriangleNeighbors* neighborBuf);

	void addPoint(Vec3 point, int replacingTriangleIndex);

	Shape toShape() const;
	IndexedShape toIndexedShape() const;
};
