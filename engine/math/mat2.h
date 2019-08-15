#pragma once

#include "vec.h"
#include "matBase.h"

template<typename T>
struct Mat2Template : public Matrix<T, 2, 2> {
	Mat2Template() : Matrix<T, 2, 2>() {}
	Mat2Template<T>(T m00, T m01, T m10, T m11) {
		T data[4]{ m00, m01, m10, m11 };
		Matrix<T, 2, 2>::setDataColMajor(data);
	}
	template<typename OtherT>
	Mat2Template<T>(const Matrix<OtherT, 2, 2>& other) : Matrix<T, 2, 2>(other) {}
};

typedef Mat2Template<double>	Mat2;
typedef Mat2Template<float>		Mat2f;
typedef Mat2Template<long long>	Mat2l;
typedef Mat2Template<double>	RotMat2;

template<typename T>
Mat2Template<T> fromAngle(T angle);