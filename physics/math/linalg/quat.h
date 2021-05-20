#pragma once

#include <stddef.h>
#include <cmath>
#include "vec.h"

namespace P3D {
template<typename T>
struct Quaternion {

	T w;
	union {
		struct {
			T i;
			T j;
			T k;
		};
		Vector<T, 3> v;
	};

	constexpr Quaternion() : w(0), i(0), j(0), k(0) {}
	constexpr Quaternion(T w, T i, T j, T k) : w(w), i(i), j(j), k(k) {}
	constexpr Quaternion(T w, Vector<T, 3> v) : w(w), v(v) {}

	template<typename OtherT>
	explicit operator Quaternion<OtherT>() const {
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

template<typename T>
Quaternion<T> operator*(const Quaternion<T>& a, const Quaternion<T>& b) {
	T w = a.w * b.w - (a.v * b.v);
	Vector<T, 3> v = a.w * b.v + a.v * b.w + (a.v % b.v);
	return Quaternion<T>(w, v);
}

// computes  q * Quaternion(0, v)
template<typename T>
Quaternion<T> operator*(const Quaternion<T>& q, const Vector<T, 3>& v) {
	return Quaternion<T>(-(v * q.v), q.w * v + q.v % v);
}

// computes  Quaternion(0, v) * q
template<typename T>
Quaternion<T> operator*(const Vector<T, 3>& v, const Quaternion<T>& q) {
	return Quaternion<T>(-(v * q.v), q.w * v + v % q.v);
}

// computes q * v * conj(q)  if q is a unit quaternion
template<typename T>
Vector<T, 3> mulQuaternionLeftRightConj(const Quaternion<T>& q, const Vector<T, 3>& v) {
	Vector<T, 3> u = q.v;
	Vector<T, 3> cross_uv = u % v;
	return v + ((cross_uv * q.w) + u % cross_uv) * static_cast<T>(2);
}

// computes conj(q) * v * q  if q is a unit quaternion
template<typename T>
Vector<T, 3> mulQuaternionLeftConjRight(const Quaternion<T>& q, const Vector<T, 3>& v) {
	Vector<T, 3> u = q.v;
	Vector<T, 3> cross_uv = v % u;
	return v + ((cross_uv * q.w) + cross_uv % u) * static_cast<T>(2);
}

template<typename T>
Quaternion<T> operator*(const Quaternion<T>& quat, T factor) {
	return Quaternion<T>(quat.w * factor, quat.v * factor);
}

template<typename T>
Quaternion<T> operator*(T factor, const Quaternion<T>& quat) {
	return Quaternion<T>(factor * quat.w, factor * quat.v);
}

template<typename T>
Quaternion<T> operator+(const Quaternion<T>& a, const Quaternion<T>& b) {
	return Quaternion<T>(a.w + b.w, a.v + b.v);
}

template<typename T>
Quaternion<T> operator-(const Quaternion<T>& a, const Quaternion<T>& b) {
	return Quaternion<T>(a.w - b.w, a.v - b.v);
}

template<typename T>
Quaternion<T> operator/(const Quaternion<T>& quat, T factor) {
	return Quaternion<T>(quat.w / factor, quat.v / factor);
}

template<typename T>
Quaternion<T> operator-(const Quaternion<T>& quat) {
	return Quaternion<T>(-quat.w, -quat.v);
}

template<typename T>
Quaternion<T>& operator+=(Quaternion<T>& quat, const Quaternion<T>& other) {
	quat.w += other.w;
	quat.v += other.v;

	return quat;
}
template<typename T>
Quaternion<T>& operator-=(Quaternion<T>& quat, const Quaternion<T>& other) {
	quat.w -= other.w;
	quat.v -= other.v;

	return quat;
}

template<typename T>
Quaternion<T>& operator*=(Quaternion<T>& quat, T factor) {
	quat.w *= factor;
	quat.v *= factor;

	return quat;
}

template<typename T>
Quaternion<T>& operator/=(Quaternion<T>& quat, T factor) {
	quat.w /= factor;
	quat.v /= factor;

	return quat;
}

template<typename T>
T dot(const Quaternion<T>& a, const Quaternion<T>& b) {
	return a.w * b.w + a.i * b.i + a.j * b.j + a.k * b.k;
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
T lengthSquared(const Quaternion<T>& q) {
	return q.w * q.w + q.i * q.i + q.j * q.j + q.k * q.k;
}

template<typename T>
T length(const Quaternion<T>& quat) {
	return std::sqrt(lengthSquared(quat));
}

template<typename T>
Quaternion<T> normalize(const Quaternion<T>& quat) {
	return quat / length(quat);
}

template<typename T>
Quaternion<T> operator~(const Quaternion<T>& quat) {
	return !quat / lengthSquared(quat);
}

template<typename T>
Quaternion<T> inverse(const Quaternion<T>& quat) {
	return ~quat;
}
};
