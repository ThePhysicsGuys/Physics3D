#include "shapeBuilder.h"

ShapeBuilder::ShapeBuilder(Vec3f * vertBuf, Triangle* triangleBuf, int vertexCount, int triangleCount, TriangleNeighbors* neighborBuf)
	: vertexBuf(vertBuf), triangleBuf(triangleBuf), vertexCount(vertexCount), triangleCount(triangleCount), neighborBuf(neighborBuf) {
	fillNeighborBuf(triangleBuf, triangleCount, neighborBuf);
}


void ShapeBuilder::addPoint(Vec3f point, int oldTriangleIndex) {
	/* To add point:
	Add point to vertex buffer
	Remove triangle that point replaces
	Add 3 new triangles connecting point to oldTriangle's neighbors
		- ABP
		- BCP
		- CAP
		With ABC the old triangle and P the new point
	update neighbors
	*/

	int newPointIndex = vertexCount;

	// add extra point
	vertexBuf[newPointIndex] = point;
	
	int t0Index = oldTriangleIndex;
	int t1Index = triangleCount;
	int t2Index = triangleCount + 1;

	Triangle oldT = triangleBuf[oldTriangleIndex];
	TriangleNeighbors oldNeighbors = neighborBuf[oldTriangleIndex];

	triangleBuf[t0Index] = Triangle{oldT[0], oldT[1], newPointIndex};
	triangleBuf[t1Index] = Triangle{oldT[1], oldT[2], newPointIndex};
	triangleBuf[t2Index] = Triangle{oldT[2], oldT[0], newPointIndex};

	vertexCount++;
	triangleCount += 2;

	neighborBuf[t0Index].BC_Neighbor = t1Index; neighborBuf[t1Index].CA_Neighbor = t0Index;
	neighborBuf[t1Index].BC_Neighbor = t2Index; neighborBuf[t2Index].CA_Neighbor = t1Index;
	neighborBuf[t2Index].BC_Neighbor = t0Index; neighborBuf[t0Index].CA_Neighbor = t2Index;

	neighborBuf[t0Index].AB_Neighbor = oldNeighbors.AB_Neighbor;
	neighborBuf[t1Index].AB_Neighbor = oldNeighbors.BC_Neighbor;
	neighborBuf[t2Index].AB_Neighbor = oldNeighbors.CA_Neighbor;

	// update neighbors next to replaced triangle
	neighborBuf[oldNeighbors.AB_Neighbor].replaceNeighbor(oldTriangleIndex, t0Index);
	neighborBuf[oldNeighbors.BC_Neighbor].replaceNeighbor(oldTriangleIndex, t1Index);
	neighborBuf[oldNeighbors.CA_Neighbor].replaceNeighbor(oldTriangleIndex, t2Index);
	
}

Polyhedron ShapeBuilder::toPolyhedron() const {
	return Polyhedron(vertexBuf, triangleBuf, vertexCount, triangleCount);
}

IndexedShape ShapeBuilder::toIndexedShape() const {
	return IndexedShape(vertexBuf, triangleBuf, vertexCount, triangleCount, neighborBuf);
}
