#include "vec2.h"

#include <cmath>

template<typename N>
N Vec2Template<N>::length() const {
	return sqrt(lengthSquared());
}

template class Vec2Template<double>;
template class Vec2Template<float>;
