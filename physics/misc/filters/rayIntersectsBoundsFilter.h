#pragma once

#include "../../math/bounds.h"
#include "../../datastructures/boundsTree.h"
#include "../../part.h"

struct RayIntersectBoundsFilter {
	Ray ray;

	RayIntersectBoundsFilter() = default;
	RayIntersectBoundsFilter(const Ray& ray) : ray(ray) {}

	bool operator()(const P3D::OldBoundsTree::TreeNode& node) const {
		return doRayAndBoundsIntersect(node.bounds, ray);
	}
	bool operator()(const Part& part) const {
		return true;
	}
};
