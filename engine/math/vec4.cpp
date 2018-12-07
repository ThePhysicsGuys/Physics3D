#include "vec4.h"

#include <cmath>

template<typename N>
N Vec4Template<N>::length() const {
	return sqrt(lengthSquared());
}

template class Vec4Template<double>;
template class Vec4Template<float>;
