#include "vec4.h"

#include <cmath>

template<typename N>
N Vec4Template<N>::length() const {
	return sqrt(lengthSquared());
}

template struct Vec4Template<double>;
template struct Vec4Template<float>;
