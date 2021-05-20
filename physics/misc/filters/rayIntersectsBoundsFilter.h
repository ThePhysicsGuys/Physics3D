#pragma once

#include "../../math/bounds.h"
#include "../../datastructures/boundsTree.h"
#include "../../part.h"

namespace P3D {
struct RayIntersectBoundsFilter {
	Ray ray;

	RayIntersectBoundsFilter() = default;
	RayIntersectBoundsFilter(const Ray& ray) : ray(ray) {}

	std::array<bool, BRANCH_FACTOR> operator()(const TreeTrunk& trunk, int trunkSize) const {
		std::array<bool, BRANCH_FACTOR> results;
		for(int i = 0; i < trunkSize; i++) {
			results[i] = doRayAndBoundsIntersect(trunk.getBoundsOfSubNode(i), this->ray);
		}
		return results;
	}
	bool operator()(const Part& part) const {
		return true;
	}
};
};
