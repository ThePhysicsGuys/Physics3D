#pragma once

#include <cmath>

template<template<typename, size_t> typename VectorBasis, typename T, typename T2, size_t Size>
auto operator*(const VectorBasis<T, Size>& a, const VectorBasis<T2, Size>& b) -> decltype(a[0]*b[0]+a[1]*b[1]) {
	decltype(a[0] * b[0] + a[1] * b[1]) result = a[0] * b[0];
	for (size_t i = 1; i < Size; i++) {
		result += a[i] * b[i];
	}
	return result;
}

template<template<typename, size_t> typename VectorBasis, typename T, typename T2, size_t Size>
auto operator+(const VectorBasis<T, Size>& a, const VectorBasis<T2, Size>& b) -> VectorBasis<decltype(a[0] + b[0]), Size> {
	VectorBasis<decltype(a[0] + b[0]), Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = a[i] + b[i];
	}
	return result;
}

template<template<typename, size_t> typename VectorBasis, typename T, typename T2, size_t Size>
auto operator-(const VectorBasis<T, Size>& a, const VectorBasis<T2, Size>& b) -> VectorBasis<decltype(a[0] - b[0]), Size> {
	VectorBasis<decltype(a[0] - b[0]), Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = a[i] - b[i];
	}
	return result;
}

template<template<typename, size_t> typename VectorBasis, typename T, typename T2, size_t Size>
auto operator*(const VectorBasis<T, Size>& vec, const T2& factor) -> VectorBasis<decltype(vec[0] * factor), Size> {
	VectorBasis<decltype(vec[0] * factor), Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = vec[i] * factor;
	}
	return result;
}

template<template<typename, size_t> typename VectorBasis, typename T, typename T2, size_t Size>
auto operator*(const T2& factor, const VectorBasis<T, Size>& vec) -> VectorBasis<decltype(factor * vec[0]), Size> {
	VectorBasis<decltype(factor * vec[0]), Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = factor * vec[i];
	}
	return result;
}

template<template<typename, size_t> typename VectorBasis, typename T, typename T2, size_t Size>
auto operator/(const VectorBasis<T, Size>& vec, const T2& factor) -> VectorBasis<decltype(vec[0] / factor), Size> {
	VectorBasis<decltype(vec[0] / factor), Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = vec[i] / factor;
	}
	return result;
}

template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
VectorBasis<T, Size> operator-(const VectorBasis<T, Size>& vec) {
	VectorBasis<T, Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = -vec[i];
	}
	return result;
}

template<template<typename, size_t> typename VectorBasis, typename T, typename T2, size_t Size>
VectorBasis<T, Size>& operator+=(VectorBasis<T, Size>& vec, const VectorBasis<T2, Size>& other) {
	for (size_t i = 0; i < Size; i++) {
		vec[i] += other[i];
	}
	return vec;
}
template<template<typename, size_t> typename VectorBasis, typename T, typename T2, size_t Size>
VectorBasis<T, Size>& operator-=(VectorBasis<T, Size>& vec, const VectorBasis<T2, Size>& other) {
	for (size_t i = 0; i < Size; i++) {
		vec[i] -= other[i];
	}
	return vec;
}
template<template<typename, size_t> typename VectorBasis, typename T, typename T2, size_t Size>
VectorBasis<T, Size>& operator*=(VectorBasis<T, Size>& vec, const T2& factor) {
	for (size_t i = 0; i < Size; i++) {
		vec[i] *= factor;
	}
	return vec;
}
template<template<typename, size_t> typename VectorBasis, typename T, typename T2, size_t Size>
VectorBasis<T, Size>& operator/=(VectorBasis<T, Size>& vec, const T2& factor) {
	for (size_t i = 0; i < Size; i++) {
		vec[i] /= factor;
	}
	return vec;
}

template<template<typename, size_t> typename VectorBasis, typename T, typename T2>
auto operator%(const VectorBasis<T, 2>& first, const VectorBasis<T2, 2>& second) -> decltype(first[0] * second[1] - first[1] * second[0]) {
	return first[0] * second[1] - first[1] * second[0];
}

template<template<typename, size_t> typename VectorBasis, typename T, typename T2>
auto operator%(const VectorBasis<T, 3>& first, const VectorBasis<T2, 3>& second) -> VectorBasis<decltype(first[1] * second[2] - first[2] * second[1]), 3> {
	return VectorBasis<decltype(first[1] * second[2] - first[2] * second[1]), 3>{
		first[1] * second[2] - first[2] * second[1],
			first[2] * second[0] - first[0] * second[2],
			first[0] * second[1] - first[1] * second[0]
	};
}

template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
T lengthSquared(const VectorBasis<T, Size>& vec) {
	T sum = vec[0] * vec[0];

	for (size_t i = 1; i < Size; i++) {
		sum += vec[i] * vec[i];
	}
	return sum;
}

template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
auto length(const VectorBasis<T, Size>& vec) {
	return sqrt(lengthSquared(vec));
}

template<template<typename, size_t> typename VectorBasis, typename T>
T length(const VectorBasis<T, 2>& vec) {
	return hypot(vec[0], vec[1]);
}

/*template<template<typename, size_t> typename VectorBasis, typename T>
T length(const VectorBasis<T, 3>& vec) {
	return hypot(vec[0], vec[1], vec[2]);
}*/

template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
VectorBasis<T, Size> normalize(const VectorBasis<T, Size>& vec) {
	return vec / length(vec);
}

template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
VectorBasis<T, Size> abs(const VectorBasis<T, Size>& vec) {
	VectorBasis<T, Size> result;
	for (size_t i = 0; i < Size; i++)
		result[i] = fabs(vec[i]);
	return result;
}

template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
bool isLongerThan(const VectorBasis<T, Size>& vec, const T& length) {
	return lengthSquared(vec) > length * length;
}

template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
bool isShorterThan(const VectorBasis<T, Size>& vec, const T& length) {
	return lengthSquared(vec) < length* length;
}

template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
VectorBasis<T, Size> withLength(const VectorBasis<T, Size>& vec, const T& newLength) {
	return vec * (newLength / length(vec));
}

template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
VectorBasis<T, Size> maxLength(const VectorBasis<T, Size>& vec, const T& maxLength) {
	if (isLongerThan(vec, maxLength))
		return withLength(vec, maxLength);
	else
		return vec;
}

template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
VectorBasis<T, Size> minLength(const VectorBasis<T, Size>& vec, const T& minLength) {
	if (isShorterThan(vec, minLength))
		return withLength(vec, minLength);
	else
		return vec;
}

/**
* used to project the result of a dotproduct back onto the original vector
* @param v the result of dot(onto, otherVec)
* @return vec * (v/lengthSquared(vec))
*/
template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
VectorBasis<T, Size> reProject(const VectorBasis<T, Size>& onto, const T& v) {
	return onto * v / lengthSquared(onto);
}

/**
* projects vec onto onto
* @param vec vector to be projected
* @param onto vector to be projected on
* @return a projected version of the given vector
*/
template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
VectorBasis<T, Size> project(const VectorBasis<T, Size>& vec, const VectorBasis<T, Size>& onto) {
	return onto * ((onto * vec) / lengthSquared(onto));
}

template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
auto angleBetween(const VectorBasis<T, Size>& first, const VectorBasis<T, Size>& second) -> decltype(acos(normalize(first)* normalize(second))) {
	return acos(normalize(first) * normalize(second));
}

/**
* returns the distance of the given point to the line that goes through the origin along this vector
* @param point
* @return the distance
*/
template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
T pointToLineDistance(const VectorBasis<T, Size>& line, const VectorBasis<T, Size>& point) {
	return length(point - project(point, line));
}

/**
* returns the squared distance of the given point to the line that goes through the origin along this vector
* @param point
* @return the square of the distance
*/
template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
T pointToLineDistanceSquared(const VectorBasis<T, Size>& line, const VectorBasis<T, Size>& point) {
	return lengthSquared(point - project(point, line));
}

template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
VectorBasis<T, Size> bisect(const VectorBasis<T, Size>& first, const VectorBasis<T, Size>& second) {
	return first * length(second) + second * length(first);
}

template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
VectorBasis<T, Size> elementWiseMul(const VectorBasis<T, Size>& first, const VectorBasis<T, Size>& second) {
	VectorBasis<T, Size> result;
	for (size_t i = 0; i < Size; i++)
		result[i] = first[i] * second[i];
	return result;
}

template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
VectorBasis<T, Size> elementWiseSquare(const VectorBasis<T, Size>& vec) {
	VectorBasis<T, Size> result;
	for (size_t i = 0; i < Size; i++)
		result[i] = vec[i] * vec[i];
	return result;
}

template<template<typename, size_t> typename VectorBasis, typename T, size_t Size>
VectorBasis<T, Size> elementWiseCube(const VectorBasis<T, Size>& vec) {
	VectorBasis<T, Size> result;
	for (size_t i = 0; i < Size; i++)
		result[i] = vec[i] * vec[i] * vec[i];
	return result;
}

template<typename T, size_t Size = 2>
struct Vec2Template {
public:
	union {
		struct {T x; T y;};
		T data[2];
	};

	Vec2Template() : x(0), y(0) {}
	explicit Vec2Template(T v) : x(v), y(v) {};
	Vec2Template(T x, T y) : x(x), y(y) {};
	template<template<typename, size_t> typename VectorBasis, typename OtherT>
	Vec2Template(const VectorBasis<OtherT, 2>& other) : x(static_cast<T>(other[0])), y(static_cast<T>(other[1])) {};
	
	T& operator[](size_t index) {return data[index];}
	const T& operator[](size_t index) const {return data[index];}
};


template<typename T, size_t Size = 3>
struct Vec3Template {
public:
	union {
		struct {T x; T y; T z;};
		T data[3];
	};

	Vec3Template() : x(0), y(0), z(0) {}
	explicit Vec3Template(T v) : x(v), y(v), z(v) {};
	Vec3Template(T x, T y, T z) : x(x), y(y), z(z) {};
	template<template<typename, size_t> typename VectorBasis, typename OtherT>
	Vec3Template(const VectorBasis<OtherT, 3>& other) : x(static_cast<T>(other[0])), y(static_cast<T>(other[1])), z(static_cast<T>(other[2])) {};
	template<template<typename, size_t> typename VectorBasis, typename OtherT, typename OtherT2>
	Vec3Template(const VectorBasis<OtherT, 2>& other, const OtherT2& z) : x(static_cast<T>(other[0])), y(static_cast<T>(other[1])), z(static_cast<T>(z)) {};
	~Vec3Template() {};

	Vec3Template<T, Size> rotateAround(const Vec3Template<T, Size> other, T angle) const {
		T s = sin(angle);
		T c = cos(angle);

		return *this* c + (other % *this) * s + other * (other * *this) * (1 - c);
	}
	
	T& operator[](size_t index) {return data[index];}
	const T& operator[](size_t index) const {return data[index];}
};

template<typename T, size_t Size = 4>
struct Vec4Template {
public:
	union {
		struct {T x; T y; T z; T w;};
		T data[4];
	};

	Vec4Template() : x(0), y(0), z(0), w(0) {};
	explicit Vec4Template(T v) : x(v), y(v), z(v), w(v) {};
	Vec4Template(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {};
	template<template<typename, size_t> typename VectorBasis, typename OtherT>
	Vec4Template(const VectorBasis<OtherT, 4>& other) : x(static_cast<T>(other[0])), y(static_cast<T>(other[1])), z(static_cast<T>(other[2])), w(static_cast<T>(other[3])) {};
	template<template<typename, size_t> typename VectorBasis, typename OtherT, typename OtherT2>
	Vec4Template(const VectorBasis<OtherT, 3>& other, const OtherT2& w) : x(static_cast<T>(other[0])), y(static_cast<T>(other[1])), z(static_cast<T>(other[2])), w(static_cast<T>(w)) {};

	template<typename T>
	explicit operator Vec3Template<T>() const {return Vec3Template<T>(x, y, z);}

	T& operator[](size_t index) {return data[index];}
	const T& operator[](size_t index) const {return data[index];}
};

typedef Vec2Template<double>	Vec2;
typedef Vec2Template<float>		Vec2f;
typedef Vec2Template<long long>	Vec2l;
typedef Vec2Template<int>		Vec2i;

typedef Vec3Template<double>	Vec3;
typedef Vec3Template<float>		Vec3f;
typedef Vec3Template<long long>	Vec3l;
typedef Vec3Template<int>		Vec3i;

typedef Vec4Template<double>	Vec4;
typedef Vec4Template<float>		Vec4f;
typedef Vec4Template<long long>	Vec4l;
typedef Vec4Template<int>		Vec4i;
