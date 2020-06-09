#pragma once

#include <stddef.h>
#include <cmath>
#include <limits>
#include "vec.h"

template<typename T>
struct Quaternion {

	union {
		struct {
			T w;
			T i;
			T j;
			T k;
		};
		struct {
			T w;
			Vector<T, 3> v;
		};
	};

	Quaternion() : w(0), i(0), j(0), k(0) {}
	Quaternion(T w, T i, T j, T k) : w(w), i(i), j(j), k(k) {}
	Quaternion(T v) : w(v), i(v), j(v), k(v) {}
	Quaternion(T w, Vector<T, 3> v) : w(w), v(v) {}

	template<typename OtherT>
	operator Quaternion<OtherT>() const {
		Quaternion<OtherT> result;

		result.w = static_cast<OtherT>(w);
		result.i = static_cast<OtherT>(i);
		result.j = static_cast<OtherT>(j);
		result.k = static_cast<OtherT>(k);

		return result;
	}

	static inline constexpr Quaternion<T> IDENTITY() {
		return Quaternion<T>(1, 0, 0, 0);
	}

	static inline constexpr Quaternion<T> ZEROS() {
		return Quaternion<T>(0, 0, 0, 0);
	}
};

typedef Quaternion<double>		Quat4;
typedef Quaternion<float>		Quat4f;
typedef Quaternion<long long>	Quat4l;
typedef Quaternion<int>			Quat4i;

template<typename T1, typename T2>
auto operator*(const Quaternion<T1>& a, const Quaternion<T2>& b) -> Quaternion<decltype(a.w * b.w - a.i * b.i)> {
	auto w = a.w * b.w - (a.v * b.v);
	auto v = a.w * b.v + a.v * b.w + (a.v % b.v);

	return Quaternion<decltype(a.w * b.w - a.i * b.i)>(w, v);
}

template<typename T1, typename T2>
auto operator*(const Quaternion<T1>& a, const Vector<T2, 3>& b) -> Vector<decltype(a.i * b.x + a.j * b.y), 3> {
	auto t = static_cast<decltype(a.i * b.x + a.j * b.y)>(2) * cross(a.v, b);
	return b + a.w * t + cross(a.v, t);
}

template<typename T1, typename T2, typename = typename std::enable_if<std::is_arithmetic<T2>::value, T2>::type>
auto operator*(const Quaternion<T1>& quat, const T2& factor) -> Quaternion<decltype(quat.w * factor)> {
	return Quaternion<decltype(quat.w * factor)>(quat.w * factor, quat.v * factor);
}

template<typename T1, typename T2, typename = typename std::enable_if<std::is_arithmetic<T1>::value, T1>::type>
auto operator*(const T1& factor, const Quaternion<T2>& quat) -> Quaternion<decltype(factor * quat.w)> {
	return Quaternion<decltype(factor * quat.w)>(factor * quat.w, factor * quat.v);
}

template<typename T1, typename T2>
auto operator+(const Quaternion<T1>& a, const Quaternion<T2>& b) -> Quaternion<decltype(a.w + b.w)> {
	return Quaternion<decltype(a.w + b.w)>(a.w + b.w, a.v + b.v);
}

template<typename T1, typename T2>
auto operator-(const Quaternion<T1>& a, const Quaternion<T2>& b) -> Quaternion<decltype(a.w - b.w)> {
	return Quaternion<decltype(a.w - b.w)>(a.w - b.w, a.v - b.v);
}

template<typename T1, typename T2, size_t Size>
auto operator/(const Quaternion<T1>& quat, const T2& factor) -> Quaternion<decltype(quat.w / factor)> {
	return Quaternion<decltype(quat.w / factor)>(quat.w / factor, quat.v / factor);
}

template<typename T>
Quaternion<T> operator-(const Quaternion<T>& quat) {
	return Quaternion<T>(-quat.w, -quat.v);
}

template<typename T1, typename T2>
Quaternion<T1>& operator+=(Quaternion<T1>& quat, const Quaternion<T2>& other) {
	quat.w += other.w;
	quat.v += other.v;

	return quat;
}

template<typename T1, typename T2>
Quaternion<T1>& operator-=(Quaternion<T1>& quat, const Quaternion<T2>& other) {
	quat.w -= other.w;
	quat.v -= other.v;

	return quat;
}

template<typename T1, typename T2>
Quaternion<T1>& operator*=(Quaternion<T1>& quat, const T2& factor) {
	quat.w *= factor;
	quat.v *= factor;

	return quat;
}
template<typename T1, typename T2>
Quaternion<T1>& operator*=(Quaternion<T1>& quat, const Quaternion<T2>& other) {
	quat = quat * other;

	return quat;
}

template<typename T1, typename T2>
Quaternion<T1>& operator/=(Quaternion<T1>& quat, const T2& factor) {
	quat.w /= factor;
	quat.v /= factor;

	return quat;
}

template<typename T>
bool operator!=(const Quaternion<T>& first, const Quaternion<T>& second) {
	return !(first == second);
}

template<typename T>
bool operator==(const Quaternion<T>& first, const Quaternion<T>& second) {
	return first.w == second.w && first.v == second.v;
}

template<typename T1, typename T2>
auto dot(const Quaternion<T1>& a, const Quaternion<T2>& b) -> decltype(a.w * b.w + a.i * b.i) {
	return a.w * b.w + a.i * b.i + a.j * b.j + a.k * b.k;
}

template<typename T>
Quaternion<T> dot(const Quaternion<T>& quat) {
	return dot(quat, quat);
}

template<typename T>
Quaternion<T> operator!(const Quaternion<T>& quat) {
	return Quaternion<T>(quat.w, -quat.v);
}

template<typename T>
Quaternion<T> conj(const Quaternion<T>& quat) {
	return !quat;
}

template<typename T>
Quaternion<T> operator~(const Quaternion<T>& quat) {
	return !quat / dot(quat);
}

template<typename T>
Quaternion<T> inverse(const Quaternion<T>& quat) {
	return ~quat;
}

template<typename T, size_t Size>
T lengthSquared(const Quaternion<T>& quat) {
	return dot(quat);
}

template<typename T>
auto length(const Quaternion<T>& quat) {
	return sqrt(lengthSquared(quat));
}

template<typename T1, typename T2>
bool isLongerThan(const Quaternion<T1>& vec, const T2& length) {
	return lengthSquared(vec) > length * length;
}

template<typename T1, typename T2>
bool isShorterThan(const Quaternion<T1>& vec, const T2& length) {
	return lengthSquared(vec) < length * length;
}

