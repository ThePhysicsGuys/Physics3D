#pragma once

#include "linalg/vec.h"
#include "linalg/mat.h"
#include "linalg/eigen.h"
#include "linalg/largeMatrix.h"
#include "cframe.h"
#include "position.h"

#include <string>
#include <sstream>

#define PI 3.14159265358979323846   // this is pi

inline double fRand(double fMin, double fMax) {
	double f = (double) rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
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
	ss.precision(4);
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
	ss.precision(4);
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
	ss.precision(4);
	ss << vector;
	return ss.str();
}

inline std::ostream& operator<<(std::ostream& os, const Position& position) {
	os << "(" << double(position.x) << ", " << double(position.y) << ", " << double(position.z) << ")";
	return os;
}

inline std::string str(const Position& position) {
	std::stringstream ss;
	ss.precision(4);
	ss << position;
	return ss.str();
}

inline std::string str(double value) {
	std::stringstream ss;
	ss.precision(4);
	ss << value;
	return ss.str();
}

inline std::string str(float value) {
	std::stringstream ss;
	ss.precision(4);
	ss << value;
	return ss.str();
}

inline std::string str(int value) {
	std::stringstream ss;
	ss.precision(4);
	ss << value;
	return ss.str();
}

template<typename N, size_t Width, size_t Height>
inline std::ostream& operator<<(std::ostream& os, const Matrix<N, Width, Height>& matrix) {
	os << "(";

	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 0; col < Width-1; col++) {
			os << matrix[row][col] << ", ";
		}
		os << matrix[row][Width-1] << "; ";
	}
	os << ")";

	return os;
}

template<typename N, size_t Size>
inline std::ostream& operator<<(std::ostream& os, const SymmetricMatrix<N, Size>& matrix) {
	os << "(";

	for(size_t row = 0; row < Size; row++) {
		for(size_t col = 0; col < Size - 1; col++) {
			os << matrix[row][col] << ", ";
		}
		os << matrix[row][Size - 1] << "; ";
	}
	os << ")";

	return os;
}

template<typename N, size_t Size>
inline std::ostream& operator<<(std::ostream& os, const DiagonalMatrix<N, Size>& matrix) {
	os << "Diag(";

	for(size_t i = 0; i < Size; i++) {
		os << matrix[i] << "; ";
	}
	os << ")";

	return os;
}

template<typename N, size_t Width, size_t Height>
inline std::string str(const Matrix<N, Width, Height>& matrix) {
	std::stringstream ss;
	ss.precision(4);
	ss << matrix;
	return ss.str();
}
template<typename N, size_t Size>
inline std::string str(const SymmetricMatrix<N, Size>& matrix) {
	std::stringstream ss;
	ss.precision(4);
	ss << matrix;
	return ss.str();
}
template<typename N, size_t Size>
inline std::string str(const DiagonalMatrix<N, Size>& matrix) {
	std::stringstream ss;
	ss.precision(4);
	ss << matrix;
	return ss.str();
}


template<typename N, size_t Size>
inline std::ostream& operator<<(std::ostream& os, const EigenValues<N, Size>& v) {
	os << "EigenValues(";
	for(size_t i = 0; i < Size - 1; i++)
		os << v[i] << ", ";

	os << v[Size - 1] << ")";
	return os;
}

template<typename N, size_t Size>
inline std::string str(const EigenValues<N, Size>& v) {
	std::stringstream ss;
	ss << v;
	return ss.str();
}


inline std::ostream& operator<<(std::ostream& os, const CFrame& cframe) {
	os << "CFrame(" << cframe.position << ", " << cframe.rotation << ")";
	return os;
}

inline std::string str(const CFrame& cframe) {
	std::stringstream ss;
	ss.precision(4);
	ss << cframe;
	return ss.str();
}
