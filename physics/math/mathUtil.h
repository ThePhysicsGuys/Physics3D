#pragma once

#include <cstdlib>
#include "linalg/vec.h"
#include "position.h"

#define PI 3.14159265358979323846   // this is pi

/*
	Returns a random number between fMin and fMax
*/
inline double fRand(double fMin, double fMax) {
	double f = (double) rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

/*

	Returns a Vec3f from the given position
*/
inline Vec3f fromPosition(const Position& position) {
	return Vec3f(position.x, position.y, position.z);
}

/*
	Returns the amount of leading zeros 
*/
inline uint64_t ctz(uint64_t n) {
#ifdef _MSC_VER
	unsigned long ret;
	_BitScanForward(&ret, n);
	return ret;
#else
	return __builtin_ctz(n);
#endif
}

/*
	Returns whether the given whole number is a power of 2
*/
template<typename T>
inline bool powOf2(T n) {
	return n && (!(n && (n - 1)));
}

/*
	Sets the xth bit of the given whole number
*/
template<typename T>
inline bool bset(T n, char x) {
	return n | (1 << x);
}

/*
	Clears the xth bit of the given whole number
*/
template<typename T>
inline bool bclear(T n, char x) {
	return n & (~(1 << x));
}

/*
	Flips the xth bit of the given whole number
*/
template<typename T>
inline bool bflip(T n, char x) {
	return n ^ (1 << x);
}