#pragma once

#include <cstdlib>

/*
	Returns a random number between fMin and fMax
*/
inline double fRand(double fMin, double fMax) {
	double f = (double) rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

/*
	Returns the amount of leading zeros 
*/
inline uint32_t ctz(uint32_t n) {
#ifdef _MSC_VER
	unsigned long ret = 0;
	_BitScanForward(&ret, static_cast<unsigned long>(n));
	return static_cast<uint32_t>(ret);
#else
	return static_cast<uint32_t>(__builtin_ctz(n));
#endif
}

/*
	Returns whether the given whole number is a power of 2
*/
template<typename T>
constexpr bool powOf2(T n) {
	return n && !(n & n - 1);
}

/*
	Sets the xth bit of the given whole number
*/
template<typename T>
constexpr void bset(T& n, char x) {
	n |= 1 << x;
}

/*
	Clears the xth bit of the given whole number
*/
template<typename T>
constexpr void bclear(T& n, char x) {
	n &= ~(1 << x);
}

/*
	Flips the xth bit of the given whole number
*/
template<typename T>
constexpr bool bflip(T& n, char x) {
	n ^= 1 << x;
}