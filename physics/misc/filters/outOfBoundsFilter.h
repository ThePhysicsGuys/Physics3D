#pragma once

#include "../../math/bounds.h"
#include "../../datastructures/boundsTree.h"
#include "../../part.h"

struct OutOfBoundsFilter {
	Bounds bounds;

	OutOfBoundsFilter() = default;
	OutOfBoundsFilter(const Bounds& bounds) : bounds(bounds) {}

	bool operator()(const TreeNode& node) const {
		return !bounds.contains(node.bounds);
	}
	bool operator()(const Part& part) const {
		return true;
	}
};
