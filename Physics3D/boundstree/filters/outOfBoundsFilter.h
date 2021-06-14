#pragma once

#include "../../math/bounds.h"
#include "../boundsTree.h"
#include "../../part.h"

namespace P3D {
struct OutOfBoundsFilter {
	Bounds bounds;

	OutOfBoundsFilter() = default;
	OutOfBoundsFilter(const Bounds& bounds) : bounds(bounds) {}

	std::array<bool, BRANCH_FACTOR> operator()(const TreeTrunk& trunk, int trunkSize) const {
		std::array<bool, BRANCH_FACTOR> results;
		for(int i = 0; i < trunkSize; i++) {
			results[i] = !this->bounds.contains(trunk.getBoundsOfSubNode(i));
		}
		return results;
	}
	bool operator()(const Part& part) const {
		return true;
	}
};
};