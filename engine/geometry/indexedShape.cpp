#include "indexedShape.h"

int& TriangleNeighbors::operator[](int index) {
	return this->neighbors[index];
}

bool TriangleNeighbors::hasNeighbor(int triangleIndex) const {
	return triangleIndex == neighbors[0] ||
		triangleIndex == neighbors[1] ||
		triangleIndex == neighbors[2];
}

int TriangleNeighbors::replaceNeighbor(int oldNeighbor, int newNeighbor) {
	for(int i = 0; i < 3; i++) {
		if(neighbors[i] == oldNeighbor) {
			neighbors[i] = newNeighbor;
			return i;
		}
	}
	throw "Neighbor not found in replaceNeighbor";
}

int TriangleNeighbors::getNeighborIndex(int neighbor) {
	for(int i = 0; i < 3; i++) {
		if(neighbors[i] == neighbor) {
			return i;
		}
	}
	throw "Neighbor not found in getNeighborIndex";
}

IndexedShape::IndexedShape(const Vec3f* vertices, SharedArrayPtr<const Triangle> triangles, int vertexCount, int triangleCount, TriangleNeighbors * neighborBuf) : Shape(vertices, triangles, vertexCount, triangleCount), neighbors(neighborBuf) {}

void fillNeighborBuf(const Triangle* triangles, int triangleCount, TriangleNeighbors* neighborBuf) {
	for(int i = 0; i < triangleCount; i++) {
		for(int j = i + 1; j < triangleCount; j++) {
			const Triangle& ti = triangles[i];
			const Triangle& tj = triangles[j];
			for(int index1 = 0; index1 < 3; index1++) {
				for(int index2 = 0; index2 < 3; index2++) {
					if(ti[index1] == tj[index2]) {
						if(ti[(index1 + 1) % 3] == tj[(index2 + 2) % 3]) {
							// Triangles are joined at edges: index1 - index1+1 <-> index2-1 - index2

							neighborBuf[i][(index1 + 2) % 3] = j;
							neighborBuf[j][(index2 + 1) % 3] = i;

						} else if(ti[(index1 + 2) % 3] == tj[(index2 + 1) % 3]) {
							// Triangles are joined at edges: index1-1 - index1 <-> index2 - index2+1

							neighborBuf[i][(index1 + 1) % 3] = j;
							neighborBuf[j][(index2 + 2) % 3] = i;
						}

						goto nextTriangle;
					}
				}
			}
			nextTriangle:
			continue;
		}
	}
}


bool IndexedShape::isValid() const {
	if(!Shape::isValid()) { return false; };

	// Assert that all neighbors are filled in
	for(int i = 0; i < triangleCount; i++)
		for(int j = 0; j < 3; j++)
			if(neighbors[i][j] < 0 || neighbors[i][j] >= triangleCount)
				return false;
	
	// Assert that, every triangle's neighbors have it as one of their neighbors
	for(int i = 0; i < triangleCount; i++) {
		TriangleNeighbors thisNeighbors = neighbors[i];
		for(int j = 0; j < 3; j++) {
			int other = thisNeighbors[j];
			if(!neighbors[other].hasNeighbor(i))
				return false;

			// check that if they ARE neighbors, then they must share vertices
			int neighborsIndex = neighbors[other].getNeighborIndex(i);
			Triangle t1 = triangles[i];
			Triangle t2 = triangles[other];
			if(!(t1[(j + 1) % 3] == t2[(neighborsIndex + 2) % 3] && t1[(j + 2) % 3] == t2[(neighborsIndex + 1) % 3]))
				return false;
		}
	}

	return true;
}
