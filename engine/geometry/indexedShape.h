#pragma once

#include "shape.h"

struct TriangleNeighbors {
	union {
		struct {
			int BC_Neighbor, CA_Neighbor, AB_Neighbor;
		};
		int neighbors[3];
	};
	int& operator[](int sideIndex);
	bool hasNeighbor(int triangleIndex) const;
	// replaces neighbor, returns index of replaced neighbor, throws BadNeighborException if not found
	int replaceNeighbor(int oldNeighbor, int newNeighbor);
	int getNeighborIndex(int neighbor);
};

/*struct TriangleIndex {
	TriangleNeighbors * neighbors;

	TriangleIndex(Triangle* triangles, int triangleCount, TriangleNeighbors * neighborBuf);
};*/

struct IndexedShape : Shape {
	TriangleNeighbors * neighbors;
	
	IndexedShape(Vec3* vertices, const Triangle* triangles, int vertexCount, int triangleCount, TriangleNeighbors * neighborBuf);

	bool isValid() const;
};

void fillNeighborBuf(const Triangle* triangles, int triangleCount, TriangleNeighbors* neighborBuf);
