#pragma once

#include "indexedShape.h"
#include "shapeBuilder.h"
#include "../datastructures/sharedArray.h"

// used in building shape
struct EdgePiece {
	unsigned int vertexIndex;
	int edgeTriangle;
	int neighborIndexOfEdgeTriangle;
};
/*
	No checks for capacity are done, make sure that vertBuf, triangleBuf and neighborBuf are large enough
*/
class ConvexShapeBuilder {
public:
	Vec3f* vertexBuf;
	Triangle* triangleBuf;
	int vertexCount;
	int triangleCount;
	TriangleNeighbors* neighborBuf;
	
	// temporary buffers
	int* removalBuffer;
	EdgePiece* newTriangleBuffer;

	ConvexShapeBuilder(Vec3f * vertBuf, Triangle* triangleBuf, int vertexCount, int triangleCount, TriangleNeighbors* neighborBuf, int* removalBuffer, EdgePiece* newTriangleBuffer);
	ConvexShapeBuilder(const Shape& s, Vec3f * newVertBuf, Triangle* newTriangleBuf, TriangleNeighbors* neighborBuf, int* removalBuffer, EdgePiece* newTriangleBuffer);

	void addPoint(const Vec3f& point, int oldTriangleIndex);
	// returns true if successful
	bool addPoint(const Vec3f& point);

	void removeTriangle(int triangleIndex);
	bool isAbove(const Vec3f& point, Triangle t);

	Shape toShape() const;
	IndexedShape toIndexedShape() const;
};
