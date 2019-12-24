#pragma once

#include <cstdlib>

#define PI 3.14159265358979323846   // this is pi

inline double fRand(double fMin, double fMax) {
	double f = (double) rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}
