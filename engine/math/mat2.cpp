#include "mat2.h"

#include <cmath>

template<typename T>
Mat2Template<T> fromAngle(T angle) {
	T sina = sin(angle);
	T cosa = cos(angle);
	return Mat2Template<T>(
		cosa, sina,
		-sina, cosa
	);
}

template struct Mat2Template<double>;
template struct Mat2Template<float>;

template Mat2Template<double> fromAngle(double);
template Mat2Template<float> fromAngle(float);
