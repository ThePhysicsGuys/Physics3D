#pragma once

#include <cstdlib>

namespace P3D {
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

template<typename T>
constexpr T bmask(char x) {
	return static_cast<T>(1) << x;
}

template<typename T, typename M>
constexpr void bmset(T& n, M mask) {
	n |= mask;
}

template<typename T>
constexpr void bset(T& n, char x) {
	bmset(n, bmask<T>(x));
}

template<typename T, typename M>
constexpr void bmclear(T& n, M mask) {
	n &= ~mask;
}

template<typename T>
constexpr void bclear(T& n, char x) {
	bmclear(n, bmask<T>(x));
}

template<typename T, typename M>
constexpr void bmflip(T& n, M mask) {
	n ^= mask;
}

template<typename T>
constexpr void bflip(T& n, char x) {
	bmflip(n, bmask<T>(x));
}

template<typename T, typename M>
constexpr bool bmtest(T n, M mask) {
	return n & mask;
}

template<typename T>
constexpr bool btest(T n, char x) {
	return bmtest(n, bmask<T>(x));
}
};