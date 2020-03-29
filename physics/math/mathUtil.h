#pragma once

#include <cstdlib>
#include "linalg/vec.h"
#include "position.h"

#define PI 3.14159265358979323846   // this is pi

inline double fRand(double fMin, double fMax) {
	double f = (double) rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

inline Vec3f fromPosition(const Position& position) {
	return Vec3f(position.x, position.y, position.z);
}