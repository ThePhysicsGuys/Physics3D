#pragma once

#include "vec3.h"
#include "position.h"
#include "bounds.h"

struct Ray {
	Position start;
	Vec3 direction;
};

/*
	Returns true if the given ray intersects the given bounds. 
	intersection and intersectionDistance will only be valid if an intersection is found

	intersection == ray.start + ray.direction * intersectionDistance
*/
inline bool doRayAndBoundsIntersect(const Bounds& bounds, const Ray& ray, Position& intersection, double& intersectionDistance) {
	// everything is computed relative to the ray origin

	Vec3Fix lMin = bounds.min - ray.start;
	Vec3Fix lMax = bounds.max - ray.start;

	Vec3 dir = ray.direction;

	{
		Vec3Fix intersectingSide = (dir.x > 0) ? lMin : lMax;
		intersectionDistance = intersectingSide.x / dir.x;
		Vec3Fix intersect = dir * intersectionDistance;
		if (intersect.y >= lMin.y && intersect.z >= lMin.z && intersect.y <= lMax.y && intersect.z <= lMax.z) {
			intersect.x = intersectingSide.x;
			intersection = ray.start + intersect;
			return true;
		}
	}
	{
		Vec3Fix intersectingSide = (dir.y > 0) ? lMin : lMax;
		intersectionDistance = intersectingSide.y / dir.y;
		Vec3Fix intersect = dir * intersectionDistance;
		if (intersect.x >= lMin.x && intersect.z >= lMin.z && intersect.x <= lMax.x && intersect.z <= lMax.z) {
			intersect.y = intersectingSide.y;
			intersection = ray.start + intersect;
			return true;
		}
	}
	{
		Vec3Fix intersectingSide = (dir.z > 0) ? lMin : lMax;
		intersectionDistance = intersectingSide.z / dir.z;
		Vec3Fix intersect = dir * intersectionDistance;
		if (intersect.x >= lMin.x && intersect.y >= lMin.y && intersect.x <= lMax.x && intersect.y <= lMax.y) {
			intersect.z = intersectingSide.z;
			intersection = ray.start + intersect;
			return true;
		}
	}
	return false;
}

inline bool doRayAndBoundsIntersect(const Bounds& bounds, const Ray& ray) {
	Position p;
	double d;
	return doRayAndBoundsIntersect(bounds, ray, p, d);
}
