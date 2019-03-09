#pragma once

#include "vec3.h"

/*
	Computes the intersection of a line along <r> starting at r0
	and the surface with normalvec n starting at s0

	With relativePos == s0-r0

	Ray expression: r0+t*r
	Plane equation: n*(p-s0) == 0

	Returns t from the ray expression
	To get the actual intersection point: p=r0+t*r
*/
template<typename N>
inline N lineSurfaceIntersection(Vec3Template<N> relativePos, Vec3Template<N> r, Vec3Template<N> n) {
	return (relativePos * n) / (r * n);
}

/*
	see rayTriangleIntersection
*/
struct RayIntersection {
	double d, s, t;
	inline bool lineIntersectsTriangle() { return s >= 0 && t >= 0 && s + t <= 1; }
	inline bool rayIntersectsTriangle() { return d >= 0 && lineIntersectsTriangle(); }
};

/*
Ray is defined as: P0 + r*P
Triangle surface is defined by: S0 + s*U + t*V

To solve:
P0 + r*P = S0 + s*U + t*V
=> s*U + t*V - r*P + R0 = 0
with R0 = P0-S0

Solving the system yields:
d = -R0 * u%v / n*P
s = -P * R0%u / n*P
t =  P * R0%v / n*P
*/
inline RayIntersection rayTriangleIntersection(Vec3 point, Vec3 ray, Vec3 v0, Vec3 v1, Vec3 v2) {
	Vec3 u = v1 - v0;
	Vec3 v = v2 - v0;

	Vec3 n = u%v;
	Vec3 surfacePos = v0;

	Vec3 relPoint = point - v0;

	double d = -relPoint * n / (n*ray); // lineSurfaceIntersection(relPoint, P, n);
	double s = -ray * (relPoint%u) / (n*ray);
	double t = ray * (relPoint%v) / (n*ray);

	return RayIntersection{d, s, t};
}

/*
	computes the squared distance of a point at relativePoint to a plane going through the origin, with normal planeNormal
	
	(planeNormal * relativePoint)*(planeNormal * relativePoint) / planeNormal.lengthSquared();
*/
inline double pointToPlaneDistanceSquared(Vec3 planeNormal, Vec3 relativePoint) {
	return (planeNormal * relativePoint)*(planeNormal * relativePoint) / planeNormal.lengthSquared();
}
