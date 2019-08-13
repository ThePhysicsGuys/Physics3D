#pragma once

#include "vec.h"

template<typename T>
struct Mat2Template {
	union {
		struct {
			T m00, m01;
			T m10, m11;
		};
		T m[4];
	};


	Mat2Template() : m00(1), m01(0), m10(0), m11(1) {};
	Mat2Template<T>(T m00, T m01, T m10, T m11) : m00(m00), m01(m01), m10(m10), m11(m11) {}
	template<typename OtherT>
	Mat2Template<T>(const Mat2Template<OtherT>& other) : m00(static_cast<T>(other.m00)), m01(static_cast<T>(other.m01)), m10(static_cast<T>(other.m10)), m11(static_cast<T>(other.m11)) {}

	Mat2Template<T> operator*(const Mat2Template<T>& other) const {
		T r00 = m00 * other.m00 + m10 * other.m01;
		T r01 = m01 * other.m00 + m11 * other.m01;
		T r10 = m00 * other.m10 + m10 * other.m11;
		T r11 = m01 * other.m10 + m11 * other.m11;

		return Mat2Template<T>(r00, r01, r10, r11);
	}
};

template<typename T>
Vec2Template<T> operator*(Mat2Template<T> m, Vec2Template<T> v) {
	return Vec2Template<T>(
		m.m00 * v.x + m.m10 * v.y,
		m.m01 * v.x + m.m11 * v.y
	);
}


typedef Mat2Template<double>	Mat2;
typedef Mat2Template<float>		Mat2f;
typedef Mat2Template<long long>	Mat2l;
typedef Mat2Template<double>	RotMat2;

template<typename T>
Mat2Template<T> fromAngle(T angle);