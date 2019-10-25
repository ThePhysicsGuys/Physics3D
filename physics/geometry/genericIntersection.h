#pragma once


#include "../math/linalg/vec.h"
#include "../math/transform.h"
#include "genericCollidable.h"

class ComputationBuffers;
struct Simplex;

struct MinkowskiPointIndices {
	Vec3f indices[2];

	Vec3f& operator[](int i) { return indices[i]; }
};

struct Simplex {
	Vec3 A, B, C, D;
	MinkowskiPointIndices At, Bt, Ct, Dt;
	int order;
	Simplex() = default;
	Simplex(Vec3 A, MinkowskiPointIndices At) : A(A), At(At), order(1) {}
	Simplex(Vec3 A, Vec3 B, MinkowskiPointIndices At, MinkowskiPointIndices Bt) : A(A), B(B), At(At), Bt(Bt), order(2) {}
	Simplex(Vec3 A, Vec3 B, Vec3 C, MinkowskiPointIndices At, MinkowskiPointIndices Bt, MinkowskiPointIndices Ct) : A(A), B(B), C(C), At(At), Bt(Bt), Ct(Ct), order(3) {}
	Simplex(Vec3 A, Vec3 B, Vec3 C, Vec3 D, MinkowskiPointIndices At, MinkowskiPointIndices Bt, MinkowskiPointIndices Ct, MinkowskiPointIndices Dt) : A(A), B(B), C(C), D(D), At(At), Bt(Bt), Ct(Ct), Dt(Dt), order(4) {}
	void insert(Vec3 newA, MinkowskiPointIndices newAt) {
		D = C;
		C = B;
		B = A;
		A = newA;
		Dt = Ct;
		Ct = Bt;
		Bt = At;
		At = newAt;
		order++;
	}
};

struct MinkPoint {
	Vec3f p;
	Vec3f originFirst;
	Vec3f originSecond;
};

struct Tetrahedron {
	MinkPoint A, B, C, D;
};

bool runGJKTransformed(const GenericCollidable& first, const GenericCollidable& second, const Transformf& relativeTransform, const Vec3f& initialSearchDirection, Tetrahedron& simp, int& iter);
bool runEPATransformed(const GenericCollidable& first, const GenericCollidable& second, const Tetrahedron& s, const Transformf& relativeTransform, Vec3f& intersection, Vec3f& exitVector, ComputationBuffers& bufs, int& iter);
