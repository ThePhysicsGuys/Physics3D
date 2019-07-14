#include "vec3.h"

#include <cmath>

template<typename N>
N Vec3Template<N>::length() const {
	return sqrt(lengthSquared());
}

template<typename N>
N Vec3Template<N>::angleBetween(const Vec3Template<N> other) const {
	return acos(this->normalize() * (other.normalize()));
}

template<typename N>
Vec3Template<N> Vec3Template<N>::abs() const {
	return Vec3Template(fabs(x), fabs(y), fabs(z));
}

template<typename N>
Vec3Template<N> Vec3Template<N>::rotateAround(const Vec3Template<N> other, N angle) const {
	N s = sin(angle);
	N c = cos(angle);

	return *this * c + (other % *this) * s + other * (other * *this) * (1 - c);
}


template struct Vec3Template<double>;
template struct Vec3Template<float>;
