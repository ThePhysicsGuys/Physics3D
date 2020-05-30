#pragma once

#include "shape.h"
#include "indexedShape.h"

class ShapeBuilder {
	Vec3f* vertexBuf;
	Triangle* triangleBuf;
	int vertexCount;
	int triangleCount;
	TriangleNeighbors* neighborBuf;

public:
	ShapeBuilder(Vec3f * vertBuf, Triangle* triangleBuf, int vertexCount, int triangleCount, TriangleNeighbors* neighborBuf);

	void addPoint(Vec3f point, int replacingTriangleIndex);

	Polyhedron toPolyhedron() const;
	IndexedShape toIndexedShape() const;
};
