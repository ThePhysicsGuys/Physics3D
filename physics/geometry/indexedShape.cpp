#include "indexedShape.h"

#include <stdexcept>
#include "../misc/validityHelper.h"

namespace P3D {
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
	throw std::runtime_error("Neighbor not found in replaceNeighbor");
}

int TriangleNeighbors::getNeighborIndex(int neighbor) {
	for(int i = 0; i < 3; i++) {
		if(neighbors[i] == neighbor) {
			return i;
		}
	}
	throw std::runtime_error("Neighbor not found in getNeighborIndex");
}

IndexedShape::IndexedShape(Polyhedron&& poly, TriangleNeighbors* neighborBuf) : Polyhedron(std::move(poly)), neighbors(neighborBuf) {}
IndexedShape::IndexedShape(const Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount, TriangleNeighbors* neighborBuf) : Polyhedron(vertices, triangles, vertexCount, triangleCount), neighbors(neighborBuf) {}

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
};


