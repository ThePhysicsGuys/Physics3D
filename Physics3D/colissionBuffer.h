#pragma once

#include <vector>

namespace P3D {
struct Colission {
	Part* p1;
	Part* p2;
	Position intersection;
	Vec3 exitVector;
};

struct ColissionBuffer {
	std::vector<Colission> freePartColissions;
	std::vector<Colission> freeTerrainColissions;

	inline void addFreePartColission(Part* a, Part* b, Position intersection, Vec3 exitVector) {
		freePartColissions.push_back(Colission{a, b, intersection, exitVector});
	}
	inline void addTerrainColission(Part* freePart, Part* terrainPart, Position intersection, Vec3 exitVector) {
		freeTerrainColissions.push_back(Colission{freePart, terrainPart, intersection, exitVector});
	}
	inline void clear() {
		freePartColissions.clear();
		freeTerrainColissions.clear();
	}
};
};

