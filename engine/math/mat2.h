#pragma once

#include "vec.h"
#include "matBase.h"

#include <cmath>

typedef Matrix<double, 2, 2>	Mat2;
typedef Matrix<float, 2, 2>		Mat2f;
typedef Matrix<long long, 2, 2>	Mat2l;
typedef Matrix<double, 2, 2>	RotMat2;

template<typename T>
Matrix<T, 2, 2> fromAngle(T angle) {
	T sina = sin(angle);
	T cosa = cos(angle);
	return Matrix<T, 2, 2>{
		cosa, -sina,
		sina, cosa
	};
}
