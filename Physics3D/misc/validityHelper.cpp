#include "validityHelper.h"

#include <vector>
#include <map>
#include <set>

#include "../geometry/polyhedron.h"
#include "../geometry/indexedShape.h"
#include "../misc/debug.h"

#include "../boundstree/boundsTree.h"
#include "../part.h"
#include "../physical.h"

namespace P3D {
bool isValidTriangle(Triangle t, int vertexCount) {
	return t.firstIndex != t.secondIndex && t.secondIndex != t.thirdIndex && t.thirdIndex != t.firstIndex &&
		t.firstIndex >= 0 && t.firstIndex < vertexCount &&
		t.secondIndex >= 0 && t.secondIndex < vertexCount &&
		t.thirdIndex >= 0 && t.thirdIndex < vertexCount;
}

// for every edge, of every triangle, check that it coincides with exactly one other triangle, in reverse order, revamped to be O(triangleCount) instead of O(triangleCount^2)
static bool isComplete(const TriangleMesh& mesh) {
	struct HitCount {
		int ascendingCount = 0;
		int descendingCount = 0;
	};
	bool verdict = true;
	// stores number of times two vertices appear as an edge, indexed by the lowest vertex
	std::vector<std::map<int, HitCount>> hitCounts(mesh.triangleCount);
	for(int i = 0; i < mesh.triangleCount; i++) {
		const Triangle& t = mesh.getTriangle(i);
		std::pair<int, int> indexPairs[]{{t.firstIndex, t.secondIndex}, {t.secondIndex, t.thirdIndex}, {t.thirdIndex, t.firstIndex}};
		for(std::pair<int, int> indexes : indexPairs) {
			if(indexes.first == indexes.second) {
				Debug::logError("Invalid triangle! Triangle %d has a duplicate index! {%d, %d, %d}", i, t.firstIndex, t.secondIndex, t.thirdIndex);
				verdict = false;
			}
			bool firstIsSmallerThanSecond = indexes.first < indexes.second;
			std::pair<int, int> sortedIndexes = firstIsSmallerThanSecond ? indexes : std::pair<int, int>{indexes.second, indexes.first};
			std::map<int, HitCount>& mp = hitCounts[sortedIndexes.first];
			auto found = mp.find(sortedIndexes.second);
			if(found == mp.end()) {
				if(firstIsSmallerThanSecond) {
					mp.emplace(sortedIndexes.second, HitCount{1,0});
				} else {
					mp.emplace(sortedIndexes.second, HitCount{0,1});
				}
			} else {
				if(firstIsSmallerThanSecond) {
					(*found).second.ascendingCount++;
				} else {
					(*found).second.descendingCount++;
				}
			}
		}
	}

	for(int firstVertex = 0; firstVertex < hitCounts.size(); firstVertex++) {
		std::map<int, HitCount>& everythingConnected = hitCounts[firstVertex];

		for(std::pair<int, HitCount> edge : everythingConnected) {
			if(edge.second.ascendingCount != 1 || edge.second.descendingCount != 1) {
				Debug::logError("Edge {%d,%d} has bad triangles connected, ascendingCount: %d, descendingCount: %d", firstVertex, edge.first, edge.second.ascendingCount, edge.second.descendingCount);
				verdict = false;
			}
		}
	}

	return verdict;
}

bool isValid(const TriangleMesh& mesh) {
	bool verdict = true;
	for(int i = 0; i < mesh.triangleCount; i++) {
		const Triangle& t = mesh.getTriangle(i);
		if(!isValidTriangle(t, mesh.vertexCount)) {
			Debug::logError("Invalid triangle! Triangle %d {%d, %d, %d} points to a nonexistent vertex or has a duplicate index!\n", i, t.firstIndex, t.secondIndex, t.thirdIndex);
			verdict = false;
		}
	}
	if(verdict == false) {
		return false;
	}
	int* usageCounts = new int[mesh.vertexCount];
	for(int i = 0; i < mesh.vertexCount; i++) {
		usageCounts[i] = 0;
	}
	for(int i = 0; i < mesh.triangleCount; i++) {
		usageCounts[mesh.getTriangle(i).firstIndex]++;
		usageCounts[mesh.getTriangle(i).secondIndex]++;
		usageCounts[mesh.getTriangle(i).thirdIndex]++;
	}
	for(int i = 0; i < mesh.vertexCount; i++) {
		if(usageCounts[i] == 0) {
			Debug::logError("Vertex %d unused!", i);
			verdict = false;
		}
	}
	delete[] usageCounts;
	return verdict;
}

bool isValid(const Polyhedron& poly) {
	if(!isValid(static_cast<const TriangleMesh&>(poly))) {
		Debug::logError("Invalid polyhedron: bad TriangleMesh\n");
		return false;
	}
	if(!isComplete(poly)) {
		Debug::logError("Invalid polyhedron: incomplete\n");
		return false;
	}

	// inverted to catch NaNs
	if(!(poly.getVolume() > 0)) {
	  
		Debug::logError("Invalid polyhedron: inverted! Volume=%f\n", poly.getVolume());
		return false;
	}
	return true;
}

bool isValid(const IndexedShape& shape) {
	if(!isValid(static_cast<const Polyhedron&>(shape))) { return false; };

	// Assert that all neighbors are filled in
	for(int i = 0; i < shape.triangleCount; i++)
		for(int j = 0; j < 3; j++)
			if(shape.neighbors[i][j] < 0 || shape.neighbors[i][j] >= shape.triangleCount)
				return false;

	// Assert that, every triangle's neighbors have it as one of their neighbors
	for(int i = 0; i < shape.triangleCount; i++) {
		TriangleNeighbors thisNeighbors = shape.neighbors[i];
		for(int j = 0; j < 3; j++) {
			int other = thisNeighbors[j];
			if(!shape.neighbors[other].hasNeighbor(i))
				return false;

			// check that if they ARE neighbors, then they must share vertices
			int neighborsIndex = shape.neighbors[other].getNeighborIndex(i);
			Triangle t1 = shape.getTriangle(i);
			Triangle t2 = shape.getTriangle(other);
			if(!(t1[(j + 1) % 3] == t2[(neighborsIndex + 2) % 3] && t1[(j + 2) % 3] == t2[(neighborsIndex + 1) % 3]))
				return false;
		}
	}

	return true;
}

static bool isConnectedPhysicalValid(const ConnectedPhysical* phys, const MotorizedPhysical* mainPhys);

static bool isPhysicalValid(const Physical* phys, const MotorizedPhysical* mainPhys) {
	if(phys->mainPhysical != mainPhys) {
		Debug::logError("Physical's parent is not mainPhys!");
		DEBUGBREAK;
		return false;
	}
	for(const Part& part : phys->rigidBody) {
		if(part.parent != phys) {
			Debug::logError("part's parent's child is not part");
			DEBUGBREAK;
			return false;
		}
	}
	for(const ConnectedPhysical& subPhys : phys->childPhysicals) {
		if(!isConnectedPhysicalValid(&subPhys, mainPhys)) return false;
	}
	return true;
}

static bool isConnectedPhysicalValid(const ConnectedPhysical* phys, const MotorizedPhysical* mainPhys) {
	return isPhysicalValid(phys, mainPhys);
}

bool isMotorizedPhysicalValid(const MotorizedPhysical* mainPhys) {
	return isPhysicalValid(mainPhys, mainPhys);
}
};
