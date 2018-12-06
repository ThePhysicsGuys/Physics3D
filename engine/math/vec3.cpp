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

template class Vec3Template<double>;
template class Vec3Template<float>;
