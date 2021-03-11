#pragma once

#include "../../math/bounds.h"
#include "../../datastructures/boundsTree.h"
#include "../../part.h"

struct RayIntersectBoundsFilter {
	Ray ray;

	RayIntersectBoundsFilter() = default;
	RayIntersectBoundsFilter(const Ray& ray) : ray(ray) {}

	bool operator()(const P3D::OldBoundsTree::TreeNode& node) const {
		return doRayAndBoundsIntersect(node.bounds, this->ray);
	}
	std::array<bool, P3D::NewBoundsTree::BRANCH_FACTOR> operator()(const P3D::NewBoundsTree::TreeTrunk& trunk, int trunkSize) const {
		std::array<bool, P3D::NewBoundsTree::BRANCH_FACTOR> results;
		for(int i = 0; i < trunkSize; i++) {
			results[i] = doRayAndBoundsIntersect(trunk.getBoundsOfSubNode(i), this->ray);
		}
		return results;
	}
	bool operator()(const Part& part) const {
		return true;
	}
};
