#pragma once

#include "math/ray.h"
#include "datastructures/boundsTree.h"
#include "part.h"

struct RayIntersectBoundsFilter {
	Ray ray;

	RayIntersectBoundsFilter(const Ray& ray) : ray(ray) {}

	bool operator()(const TreeNode& node) const {
		Position p;
		double d;
		return doRayAndBoundsIntersect(node.bounds, ray, p, d);
	}
	bool operator()(const Part& part) const {
		return true;
	}
};
