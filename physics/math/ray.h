#pragma once

#include "linalg/vec.h"
#include "position.h"
#include "bounds.h"

//#include <optional>

struct Ray {
	Position origin;
	Vec3 direction;
};

/*
	Returns if the given ray intersects the given bounds. 

	Slight redundancy: 
		intersection == ray.start + ray.direction * intersectionDistance

	But intersection is modified to lie exactly on the bounds boundry
*/
/*struct RayBoundsIntersection {
	Position intersection;
	double intersectionDistance;	
};
inline std::optional<RayBoundsIntersection> computeRayBoundsIntersection(const Bounds& bounds, const Ray& ray) {
	// everything is computed relative to the ray origin

	Vec3Fix lMin = bounds.min - ray.start;
	Vec3Fix lMax = bounds.max - ray.start;

	Vec3 dir = ray.direction;

	{
		Vec3Fix intersectingSide = (dir.x > 0) ? lMin : lMax;
		double intersectionDistance = static_cast<double>(intersectingSide.x) / dir.x;
		Vec3Fix intersect = dir * intersectionDistance;
		if (intersect.y >= lMin.y && intersect.z >= lMin.z && intersect.y <= lMax.y && intersect.z <= lMax.z) {
			intersect.x = intersectingSide.x;
			return RayBoundsIntersection{ray.start + intersect, intersectionDistance};
		}
	}
	{
		Vec3Fix intersectingSide = (dir.y > 0) ? lMin : lMax;
		double intersectionDistance = static_cast<double>(intersectingSide.y) / dir.y;
		Vec3Fix intersect = dir * intersectionDistance;
		if (intersect.x >= lMin.x && intersect.z >= lMin.z && intersect.x <= lMax.x && intersect.z <= lMax.z) {
			intersect.y = intersectingSide.y;
			return RayBoundsIntersection{ray.start + intersect, intersectionDistance};
		}
	}
	{
		Vec3Fix intersectingSide = (dir.z > 0) ? lMin : lMax;
		double intersectionDistance = static_cast<double>(intersectingSide.z) / dir.z;
		Vec3Fix intersect = dir * intersectionDistance;
		if (intersect.x >= lMin.x && intersect.y >= lMin.y && intersect.x <= lMax.x && intersect.y <= lMax.y) {
			intersect.z = intersectingSide.z;
			return RayBoundsIntersection{ray.start + intersect, intersectionDistance};
		}
	}
	return {};
}*/

/*
	Returns true if the given ray intersects the given bounds. 
*/
inline bool doRayAndBoundsIntersect(const Bounds& bounds, const Ray& ray) {
	// everything is computed relative to the ray origin

	Vec3Fix lMin = bounds.min - ray.origin;
	Vec3Fix lMax = bounds.max - ray.origin;

	Vec3 dir = ray.direction;

	{
		Vec3Fix intersectingSide = (dir.x > 0) ? lMin : lMax;
		double intersectionDistance = static_cast<double>(intersectingSide.x) / dir.x;
		Vec3Fix intersect = dir * intersectionDistance;
		if (intersect.y >= lMin.y && intersect.z >= lMin.z && intersect.y <= lMax.y && intersect.z <= lMax.z) {
			return true;
		}
	}
	{
		Vec3Fix intersectingSide = (dir.y > 0) ? lMin : lMax;
		double intersectionDistance = static_cast<double>(intersectingSide.y) / dir.y;
		Vec3Fix intersect = dir * intersectionDistance;
		if (intersect.x >= lMin.x && intersect.z >= lMin.z && intersect.x <= lMax.x && intersect.z <= lMax.z) {
			return true;
		}
	}
	{
		Vec3Fix intersectingSide = (dir.z > 0) ? lMin : lMax;
		double intersectionDistance = static_cast<double>(intersectingSide.z) / dir.z;
		Vec3Fix intersect = dir * intersectionDistance;
		if (intersect.x >= lMin.x && intersect.y >= lMin.y && intersect.x <= lMax.x && intersect.y <= lMax.y) {
			return true;
		}
	}
	return false;
}

