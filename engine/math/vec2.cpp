#include "vec2.h"

#include <cmath>

template<typename N>
N Vec2Template<N>::length() const {
	return sqrt(lengthSquared());
}

template struct Vec2Template<double>;
template struct Vec2Template<float>;
