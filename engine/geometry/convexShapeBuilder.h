#pragma once

#include "indexedShape.h"
#include "shapeBuilder.h"

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
	Vec3* vertexBuf;
	Triangle* triangleBuf;
	int vertexCount;
	int triangleCount;
	TriangleNeighbors* neighborBuf;
	
	// temporary buffers
	int* removalBuffer;
	EdgePiece* newTriangleBuffer;

	ConvexShapeBuilder(Vec3 * vertBuf, Triangle* triangleBuf, int vertexCount, int triangleCount, TriangleNeighbors* neighborBuf, int* removalBuffer, EdgePiece* newTriangleBuffer);
	ConvexShapeBuilder(const Shape s, Vec3 * newVertBuf, Triangle* newTriangleBuf, TriangleNeighbors* neighborBuf, int* removalBuffer, EdgePiece* newTriangleBuffer);

	void addPoint(Vec3 point, int oldTriangleIndex);
	// returns true if successful
	bool addPoint(Vec3 point);

	void removeTriangle(int triangleIndex);
	bool isAbove(Vec3 point, Triangle t);

	Shape toShape() const;
	IndexedShape toIndexedShape() const;
};
