#include "convexShapeBuilder.h"

#include <algorithm>

#include "../misc/validityHelper.h"
#include <assert.h>

ConvexShapeBuilder::ConvexShapeBuilder(Vec3f * vertBuf, Triangle* triangleBuf, int vertexCount, int triangleCount, TriangleNeighbors* neighborBuf, int* removalBuffer, EdgePiece* newTriangleBuffer)
	: vertexBuf(vertBuf), triangleBuf(triangleBuf), vertexCount(vertexCount), triangleCount(triangleCount), neighborBuf(neighborBuf), removalBuffer(removalBuffer), newTriangleBuffer(newTriangleBuffer) {
	fillNeighborBuf(triangleBuf, triangleCount, neighborBuf);
}

ConvexShapeBuilder::ConvexShapeBuilder(const Polyhedron& s, Vec3f * vertBuf, Triangle* triangleBuf, TriangleNeighbors* neighborBuf, int* removalBuffer, EdgePiece* newTriangleBuffer)
	: vertexBuf(vertBuf), triangleBuf(triangleBuf), vertexCount(s.vertexCount), triangleCount(s.triangleCount), neighborBuf(neighborBuf), removalBuffer(removalBuffer), newTriangleBuffer(newTriangleBuffer) {

	for(int i = 0; i < s.vertexCount; i++) {
		vertBuf[i] = s[i];
	}

	for (int i = 0; i < s.triangleCount; i++) {
		triangleBuf[i] = s.getTriangle(i);
	}

	fillNeighborBuf(triangleBuf, triangleCount, neighborBuf);
}

void moveTriangle(Triangle* triangleBuf, TriangleNeighbors* neighborBuf, int oldIndex, int newIndex) {
	triangleBuf[newIndex] = triangleBuf[oldIndex];
	TriangleNeighbors neighbors = neighborBuf[oldIndex];
	neighborBuf[newIndex] = neighbors;

	// update neighbors of moved triangle
	for(int neighborIndex : neighbors.neighbors) {
		neighborBuf[neighborIndex].replaceNeighbor(oldIndex, newIndex);
	}
}

void ConvexShapeBuilder::removeTriangle(int triangleIndex) {
	Triangle oldT = triangleBuf[triangleIndex];
	TriangleNeighbors& oldN = neighborBuf[triangleIndex];

	// update neighbors of deleted triangle
	for(int neighbor : oldN.neighbors) {
		oldN.replaceNeighbor(triangleIndex, -1);
	}

	// move last triangle in the list to the spot of the deleted triangle
	triangleCount--;
	moveTriangle(triangleBuf, neighborBuf, triangleCount, triangleIndex);
}

bool ConvexShapeBuilder::isAbove(const Vec3f& point, Triangle t) {
	Vec3f* verts = vertexBuf;
	Vec3f v0 = verts[t[0]];
	Vec3f v1 = verts[t[1]];
	Vec3f v2 = verts[t[2]];
	Vec3f normalVec = (v1 - v0) % (v2 - v0);

	return (point - v0) * normalVec > 0;
}


struct ConvexTriangleIterator {
	int* removalList;
	EdgePiece* newTrianglesList;

	int newTriangleCount = 0;
	int removalCount = 0;

	Vec3f point;
	ConvexShapeBuilder& shapeBuilder;

	ConvexTriangleIterator(const Vec3f& point, ConvexShapeBuilder& shapeBuilder, int* removalBuffer, EdgePiece* newTrianglesBuffer) : point(point), shapeBuilder(shapeBuilder), removalList(removalBuffer), newTrianglesList(newTrianglesBuffer) {}

	void markTriangleRemoved(int triangle) {
		shapeBuilder.neighborBuf[triangle].AB_Neighbor = -1;
		removalList[removalCount++] = triangle;
	}

	bool isRemoved(int triangle) {
		return shapeBuilder.neighborBuf[triangle].AB_Neighbor == -1;
	}

	bool isAbove(Triangle t) {
		Vec3f* verts = shapeBuilder.vertexBuf;
		Vec3f v0 = verts[t[0]];
		Vec3f v1 = verts[t[1]];
		Vec3f v2 = verts[t[2]];
		Vec3f normalVec = (v1 - v0) % (v2 - v0);

		return (point - v0) * normalVec > 0;
	}

	// returned value is whether the last triangle checked was an edgePiece, in that case add it to the list
	void recurseTriangle(int currentTriangle, int previousTriangle, int previousTriangleSide) {
		if(isRemoved(currentTriangle)) return;

		Triangle curTriangle = shapeBuilder.triangleBuf[currentTriangle];

		TriangleNeighbors currentNeighbors = shapeBuilder.neighborBuf[currentTriangle];
		int arrivingNeighborIndex = currentNeighbors.getNeighborIndex(previousTriangle);

		if(isAbove(curTriangle)) {
			// remove this triangle, check neighbors

			markTriangleRemoved(currentTriangle);

			int rightSide = (arrivingNeighborIndex + 1) % 3;
			int leftSide = (arrivingNeighborIndex + 2) % 3;

			recurseTriangle(currentNeighbors[rightSide], currentTriangle, rightSide);

			recurseTriangle(currentNeighbors[leftSide], currentTriangle, leftSide);
		} else {
			// this triangle is an edge, add it to the list
			
			int vertexInTriangleIndex = (previousTriangleSide + 2) % 3;
			int vertex = shapeBuilder.triangleBuf[previousTriangle][vertexInTriangleIndex];

			newTrianglesList[newTriangleCount++] = EdgePiece{vertex, currentTriangle, arrivingNeighborIndex};
		}
	}

	void linkTipTriangles(int next, int previous) {
		shapeBuilder.neighborBuf[previous].CA_Neighbor = next;
		shapeBuilder.neighborBuf[next].AB_Neighbor = previous;
	}

	void createTipTriangle(int newPointVertex, int previousVertex, int triangleToBeReplaced, EdgePiece replacingInfo) {
		// int toBeReplaced = removalList[i];
		// EdgePiece replacingInfo = newTrianglesList[i];

		int edgeTriangle = replacingInfo.edgeTriangle;

		shapeBuilder.triangleBuf[triangleToBeReplaced] = Triangle{newPointVertex, previousVertex, replacingInfo.vertexIndex};
		
		shapeBuilder.neighborBuf[triangleToBeReplaced].BC_Neighbor = edgeTriangle;
		shapeBuilder.neighborBuf[edgeTriangle][replacingInfo.neighborIndexOfEdgeTriangle] = triangleToBeReplaced;
	}

	void applyUpdates(int newPointVertex) {
		int easilyMovableTriangles = std::min(newTriangleCount, removalCount);

		int previousVertex = newTrianglesList[newTriangleCount-1].vertexIndex;
		int previousTriangle = -1;
		
		{
			int toBeReplaced = removalList[0];
			EdgePiece replacingInfo = newTrianglesList[0];

			createTipTriangle(newPointVertex, previousVertex, toBeReplaced, replacingInfo);

			previousVertex = replacingInfo.vertexIndex;
			previousTriangle = toBeReplaced;
		}

		for(int i = 1; i < easilyMovableTriangles; i++) {
			int toBeReplaced = removalList[i];
			EdgePiece replacingInfo = newTrianglesList[i];

			createTipTriangle(newPointVertex, previousVertex, toBeReplaced, replacingInfo);

			linkTipTriangles(toBeReplaced, previousTriangle);

			previousVertex = replacingInfo.vertexIndex;
			previousTriangle = toBeReplaced;
		}

		if(newTriangleCount >= removalCount) {
			// extra triangles, add at the end

			for(int i = removalCount; i < newTriangleCount; i++) {
				int toBeReplaced = shapeBuilder.triangleCount++;
				EdgePiece replacingInfo = newTrianglesList[i];

				createTipTriangle(newPointVertex, previousVertex, toBeReplaced, replacingInfo);

				linkTipTriangles(toBeReplaced, previousTriangle);

				previousVertex = replacingInfo.vertexIndex;
				previousTriangle = toBeReplaced;
			}


			linkTipTriangles(removalList[0], previousTriangle);
		} else {
			// not enough triangles to fill the gaps, move stuff back
			linkTipTriangles(removalList[0], previousTriangle);



			// The shape is entirely valid at this point, we just need to remove the triangles that are no longer part of it

			int resultingPolygonSize = shapeBuilder.triangleCount + newTriangleCount - removalCount;

			int replacingTriangleCursor = resultingPolygonSize;

			for(int i = newTriangleCount; i < removalCount; i++) {
				int triangleToRemove = removalList[i];

				if(triangleToRemove < resultingPolygonSize) {
					for(; replacingTriangleCursor < shapeBuilder.triangleCount; replacingTriangleCursor++) {
						if(!isRemoved(replacingTriangleCursor)) {
							moveTriangle(shapeBuilder.triangleBuf, shapeBuilder.neighborBuf, replacingTriangleCursor, triangleToRemove);
							replacingTriangleCursor++;
							goto nextTriangle;
						}
					}
					throw "Bad state in convexShapeBuilder! could not find non removed triangle to fill the gap!";
				}
				nextTriangle:;
			}

			shapeBuilder.triangleCount = resultingPolygonSize;
		}
	}
};

void ConvexShapeBuilder::addPoint(const Vec3f& point, int oldTriangleIndex) {
	assert(isVecValid(point));

	ConvexTriangleIterator iter(point, *this, this->removalBuffer, this->newTriangleBuffer);

	TriangleNeighbors neighbors = neighborBuf[oldTriangleIndex];

	iter.markTriangleRemoved(oldTriangleIndex);
	
	iter.recurseTriangle(neighbors[0], oldTriangleIndex, 0);
	iter.recurseTriangle(neighbors[1], oldTriangleIndex, 1);
	iter.recurseTriangle(neighbors[2], oldTriangleIndex, 2);

	vertexBuf[vertexCount++] = point;
	iter.applyUpdates(vertexCount-1);
}

bool ConvexShapeBuilder::addPoint(const Vec3f& point) {
	for(int i = 0; i < triangleCount; i++) {
		if(isAbove(point, triangleBuf[i])) {
			addPoint(point, i);
			return true;
		}
	}
	return false;
}

Polyhedron ConvexShapeBuilder::toShape() const {
	return Polyhedron(vertexBuf, triangleBuf, vertexCount, triangleCount);
}
IndexedShape ConvexShapeBuilder::toIndexedShape() const {
	return IndexedShape{vertexBuf, triangleBuf, vertexCount, triangleCount, neighborBuf};
}
