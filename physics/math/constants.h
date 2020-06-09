#pragma once

#include <limits>

#define EPSILON(T) epsilon<T>()
#define PI pi<double>()
#define E e<double>()
#define G g<double>()
#define HALF_PI half_pi<double>()
#define TWO_PI two_pi<double>()
#define GOLDEN_RATIO golden_ratio<double>()
#define SQ2_2 sq2_2<double>()
#define SQ3_2 sq3_2<double>()

template<typename T>
constexpr T epsilon() {
	return std::numeric_limits<T>::epsilon();
}

template<typename T>
constexpr T pi() {
	return static_cast<T>(3.14159265358979323846264338327950288);
}

template<typename T>
constexpr T two_pi() {
	return static_cast<T>(6.28318530717958647692528676655900576);
}

template<typename T>
constexpr T half_pi() {
	return static_cast<T>(1.57079632679489661923132169163975144);
}

template<typename T>
constexpr T e() {
	return static_cast<T>(2.71828182845904523536028747135266249);
}

template<typename T>
constexpr T g() {
	return static_cast<T>(1.61803398874989484820458683436563811772030917980576286213544862270526046281890);
}

template<typename T>
constexpr T golden_ratio() {
	return static_cast<T>(1.61803398874989484820458683436563811);
}

template<typename T>
constexpr T sq2_2() {
	return static_cast<T>(0.70710678118654752440084436210484903);
}

template<typename T>
constexpr T sq3_2() {
	return static_cast<T>(0.86602540378443864676372317075293618);
}