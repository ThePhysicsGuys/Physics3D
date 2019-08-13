#pragma once

#include "vec.h"
#include "mat2.h"
#include "mat3.h"
#include "mat4.h"
#include "cframe.h"
#include "position.h"

#include <string>
#include <sstream>

#define PI 3.14159265358979323846   // this is pi

inline double fRand(double fMin, double fMax) {
	double f = (double) rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

template<typename N>
inline std::ostream& operator<<(std::ostream& os, const Mat4Template<N>& matrix) {
	os << "(\n\t";
	for (int i = 0; i < 15; i++) {
		os << matrix.m[i];
		os << (((i + 1) % 4 == 0) ? ";\n\t" : ", ");
	}
	os << matrix.m[15] << ")";

	return os;
}

template<typename N>
inline std::string str(const Mat4Template<N>& matrix) {
	std::stringstream ss;
	ss << matrix;
	return ss.str();
}

template<typename N>
inline std::ostream& operator<<(std::ostream& os, const Vec4Template<N>& vector) {
	os << "(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")";
	return os;
}

template<typename N>
inline std::string str(const Vec4Template<N>& vector) {
	std::stringstream ss;
	ss << vector;
	return ss.str();
}


template<template<typename> typename Mat, typename N, typename std::enable_if<std::is_base_of<M33Type, Mat<N>>::value>::type* = nullptr>
inline std::ostream& operator<<(std::ostream& os, const Mat<N>& matrix) {
	os << "(\n\t";

	os << matrix.m00 << ", " << matrix.m01 << ", " << matrix.m02 << ";\n\t";
	os << matrix.m10 << ", " << matrix.m11 << ", " << matrix.m12 << ";\n\t";
	os << matrix.m20 << ", " << matrix.m21 << ", " << matrix.m22 << ";\n)";

	return os;
}


template<template<typename> typename Mat, typename N, typename std::enable_if<std::is_base_of<M33Type, Mat<N>>::value>::type* = nullptr>
inline std::string str(const Mat<N>& matrix) {
	std::stringstream ss;
	ss << matrix;
	return ss.str();
}

template<typename N>
inline std::ostream& operator<<(std::ostream& os, const Vec3Template<N>& vector) {
	os << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
	return os;
}

template<typename N>
inline std::string str(const Vec3Template<N>& vector) {
	std::stringstream ss;
	ss << vector;
	return ss.str();
}

inline std::ostream& operator<<(std::ostream& os, const Position& position) {
	os << "(" << double(position.x) << ", " << double(position.y) << ", " << double(position.z) << ")";
	return os;
}

inline std::string str(const Position& position) {
	std::stringstream ss;
	ss << position;
	return ss.str();
}

template<typename N>
inline std::ostream& operator<<(std::ostream& os, const Mat2Template<N>& matrix) {
	os << "(\n\t";

	os << matrix.m[0] << ", " << matrix.m[1] << ";\n\t";
	os << matrix.m[2] << ", " << matrix.m[3] << ")";

	return os;
}

template<typename N>
inline std::string str(const Mat2Template<N>& matrix) {
	std::stringstream ss;
	ss << matrix;
	return ss.str();
}

template<typename N>
inline std::ostream& operator<<(std::ostream& os, const Vec2Template<N>& vector) {
	os << "(" << vector.x << ", " << vector.y << ")";
	return os;
}

template<typename N>
inline std::string str(const Vec2Template<N>& vector) {
	std::stringstream ss;
	ss << vector;
	return ss.str();
}

template<typename N>
inline std::ostream& operator<<(std::ostream& os, const EigenValues<N>& v) {
	os << "EigenValues(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
	return os;
}

template<typename N>
inline std::string str(const EigenValues<N>& v) {
	std::stringstream ss;
	ss << v;
	return ss.str();
}

std::ostream& operator<<(std::ostream& os, const CFrame& cframe);
std::string str(const CFrame& cframe);


