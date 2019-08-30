#pragma once

#include "vec.h"
#include "mat2.h"
#include "mat3.h"
#include "mat4.h"
#include "largeMatrix.h"
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

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const LargeMatrix<T>& matrix) {
	for (int i = 0; i < matrix.height; i++) {
		for (int j = 0; j < matrix.width; j++) {
			os << matrix[i][j] << '\t';
		}
		os << '\n';
	}

	return os;
}

template<typename T>
inline std::string str(const LargeMatrix<T>& matrix) {
	std::stringstream ss;
	ss << matrix;
	return ss.str();
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const LargeVector<T>& vector) {
	for (int i = 0; i < vector.size; i++) {
		os << vector[i] << ',';
	}

	return os;
}

template<typename T>
inline std::string str(const LargeVector<T>& vector) {
	std::stringstream ss;
	ss << vector;
	return ss.str();
}

template<typename T, size_t Size>
inline std::ostream& operator<<(std::ostream& os, const Vector<T, Size>& vector) {
	os << '(';
	for (size_t i = 0; i < Size - 1; i++) {
		os << vector[i] << ", ";
	}
	os << vector[Size - 1] << ")";
	return os;
}

template<typename T, size_t Size>
inline std::string str(const Vector<T, Size>& vector) {
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


