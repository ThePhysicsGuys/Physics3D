#pragma once

#include "../../math/bounds.h"
#include "../../datastructures/boundsTree.h"
#include "../../part.h"

struct OutOfBoundsFilter {
	Bounds bounds;

	OutOfBoundsFilter() = default;
	OutOfBoundsFilter(const Bounds& bounds) : bounds(bounds) {}

	bool operator()(const P3D::OldBoundsTree::TreeNode& node) const {
		return !this->bounds.contains(node.bounds);
	}
	std::array<bool, P3D::NewBoundsTree::BRANCH_FACTOR> operator()(const P3D::NewBoundsTree::TreeTrunk& trunk, int trunkSize) const {
		std::array<bool, P3D::NewBoundsTree::BRANCH_FACTOR> results;
		for(int i = 0; i < trunkSize; i++) {
			results[i] = !this->bounds.contains(trunk.getBoundsOfSubNode(i));
		}
		return results;
	}
	bool operator()(const Part& part) const {
		return true;
	}
};
