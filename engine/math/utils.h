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
template<typename T>
struct RayIntersection {
	T d, s, t;
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
template<typename T>
inline RayIntersection<T> rayTriangleIntersection(Vec3Template<T> point, Vec3Template<T> ray, Vec3Template<T> v0, Vec3Template<T> v1, Vec3Template<T> v2) {
	Vec3Template<T> u = v1 - v0;
	Vec3Template<T> v = v2 - v0;

	Vec3Template<T> n = u%v;
	Vec3Template<T> surfacePos = v0;

	Vec3Template<T> relPoint = point - v0;

	T d = -relPoint * n / (n*ray); // lineSurfaceIntersection(relPoint, P, n);
	T s = -ray * (relPoint%u) / (n*ray);
	T t = ray * (relPoint%v) / (n*ray);

	return RayIntersection<T>{d, s, t};
}

/*
	computes the squared distance of a point at relativePoint to a plane going through the origin, with normal planeNormal
	
	(planeNormal * relativePoint)*(planeNormal * relativePoint) / planeNormal.lengthSquared();
*/
inline double pointToPlaneDistanceSquared(Vec3 planeNormal, Vec3 relativePoint) {
	return (planeNormal * relativePoint)*(planeNormal * relativePoint) / planeNormal.lengthSquared();
}

inline float pointToPlaneDistanceSquared(Vec3f planeNormal, Vec3f relativePoint) {
	return (planeNormal * relativePoint)*(planeNormal * relativePoint) / planeNormal.lengthSquared();
}

struct CrossOverPoint {
	double s;
	double t;
};

/*
	Get where the two lines are closest to eachother
	P := P0+s*U
	Q := Q0+t*V

	With W0 = P0-Q0
*/
inline CrossOverPoint getNearestCrossoverOfRays(Vec3 U, Vec3 V, Vec3 W0) {
	double a = U*U;
	double b = U*V;
	double c = V*V;
	double d = U*W0;
	double e = V*W0;

	double s = (b*e - c*d) / (a*c - b*b);
	double t = (a*e - b*d) / (a*c - b*b);

	return CrossOverPoint{s, t};
}
