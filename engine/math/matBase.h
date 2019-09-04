#pragma once

#include "vec.h"
#include <type_traits>
#include <initializer_list>

template<typename T, size_t Stride>
struct ArrayWithStride {
	T* startingElement;
	T& operator[](size_t index) const { return startingElement[index * Stride]; }
};
template<typename T, size_t Stride>
struct ArrayWithStrideConst {
	const T* startingElement;
	const T& operator[](size_t index) const { return startingElement[index * Stride]; }
};

template<typename T, size_t Width, size_t Height>
struct Matrix {
	T data[Width * Height];

	Matrix<T, Width, Height>() {
		for (size_t i = 0; i < Width * Height; i++) {
			this->data[i] = 0;
		}
	}

	// construction with initializer list
	// initializer list must always be ROW-MAJOR
	Matrix<T, Width, Height>(const std::initializer_list<T>& list) {
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < Width; col++) {
				(*this)[row][col] = list[row * Width + col];
			}
		}
	}

	void setDataRowMajor(const T* data) {
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < Width; col++) {
				(*this)[row][col] = data[row * Width + col];
			}
		}
	}
	void setDataColMajor(const T* data) {
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < Width; col++) {
				(*this)[row][col] = data[row + col * Height];
			}
		}
	}

	template<typename T2>
	Matrix<T, Width, Height>(const Matrix<T2, Width, Height>& mat) {
		for (size_t i = 0; i < Width * Height; i++) {
			this->data[i] = static_cast<T>(mat.data[i]);
		}
	}

	Matrix<T, Height, Width> transpose() const {
		Matrix<T, Height, Width> result;
		for (size_t row = 0; row < Height; row++) {
			for (size_t col = 0; col < Width; col++) {
				result[col][row] = (*this)[row][col];
			}
		}
		return result;
	}

	template<size_t SubWidth, size_t SubHeight>
	Matrix<T, SubWidth, SubHeight> getSubMatrix(size_t topLeftRow, size_t topLeftCol) {
		Matrix<T, SubWidth, SubHeight> result;

		for (size_t row = 0; row < SubHeight; row++) {
			for (size_t col = 0; col < SubWidth; col++) {
				result[row][col] = (*this)[row+topLeftRow][col+topLeftCol];
			}
		}
		return result;
	}

	/*template<std::enable_if_t<Width == Height, size_t> Size = Width>
	T det() const {
		if constexpr (Size == 1) {
			return data[0];
		}
		if constexpr (Size == 2) {
			return data[0] * data[3] - data[1] * data[2];
		}
		T total = 0;
		for (size_t col = 0; col < Size; col++) {
			Matrix<T, Size - 1, Size - 1> minorMat;
			for (size_t row = 0; row < Size - 1; row++) {
				for (size_t j = 0; j < col; j++) {
					minorMat[row][j] = (*this)[row + 1][j];
				}
				// skip i-th row
				for (size_t j = row; j < Size-1; j++) {
					minorMat[row][j] = (*this)[row + 1][j+1];
				}
			}
			T detOfMinor = minorMat.det();
			if (col % 2 == 0) {
				total += detOfMinor * (*this)[0][col];
			} else {
				total -= detOfMinor * (*this)[0][col];
			}
		}
		return total;
	}*/

	// Returns a row of this matrix
	ArrayWithStride<T, Width> operator[](size_t index) { return ArrayWithStride<T, Width>{ data + index }; }
	// Returns a row of this matrix
	ArrayWithStrideConst<T, Width> operator[](size_t index) const { return ArrayWithStrideConst<T, Width>{ data + index }; }
};

template<typename T, size_t Size>
using SquareMatrix = Matrix<T, Size, Size>;

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height> fromColMajorData(const T* data) {
	Matrix<T, Width, Height> mat;
	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 0; col < Width; col++) {
			mat[row][col] = data[row + col * Height];
		}
	}
	return mat;
}

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height> fromRowMajorData(const T* data) {
	Matrix<T, Width, Height> mat;
	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 0; col < Width; col++) {
			mat[row][col] = data[row * Width + col];
		}
	}
	return mat;
}

template<template<typename, size_t> typename VectorBasis, typename T, size_t Width, size_t Height>
VectorBasis<T, Height> operator*(const Matrix<T, Width, Height>& m, const VectorBasis<T, Width>& v) {
	VectorBasis<T, Height> result;

	for (size_t row = 0; row < Height; row++) {
		T sum = m[row][0] * v[0];
		for (size_t col = 1; col < Width; col++) {
			sum += m[row][col] * v[col];
		}
		result[row] = sum;
	}
	return result;
}

template<typename T, size_t ResultWidth, size_t ResultHeight, size_t Josize_tSize>
Matrix<T, ResultWidth, ResultHeight> operator*(const Matrix<T, Josize_tSize, ResultHeight>& m1, const Matrix<T, ResultWidth, Josize_tSize>& m2) {
	Matrix<T, ResultWidth, ResultHeight> result;

	for (size_t m2Col = 0; m2Col < ResultWidth; m2Col++) {
		for (size_t row = 0; row < ResultHeight; row++) {
			T sum = m1[row][0] * m2[0][m2Col];
			for (size_t col = 1; col < Josize_tSize; col++) {
				sum += m1[row][col] * m2[col][m2Col];
			}
			result[row][m2Col] = sum;
		}
	}
	return result;
}

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height> operator+(const Matrix<T, Width, Height>& m1, const Matrix<T, Width, Height>& m2) {
	Matrix<T, Width, Height> result;

	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 1; col < Width; col++) {
			result[row][col] = m1[row][col] + m2[row][col];
		}
	}
	return result;
}

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height> operator-(const Matrix<T, Width, Height>& m1, const Matrix<T, Width, Height>& m2) {
	Matrix<T, Width, Height> result;

	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 1; col < Width; col++) {
			result[row][col] = m1[row][col] - m2[row][col];
		}
	}
	return result;
}

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height> operator*(const Matrix<T, Width, Height>& m1, const T& factor) {
	Matrix<T, Width, Height> result;

	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 1; col < Width; col++) {
			result[row][col] = m1[row][col] * factor;
		}
	}
	return result;
}

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height> operator*(const T& factor, const Matrix<T, Width, Height>& m1) {
	Matrix<T, Width, Height> result;

	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 1; col < Width; col++) {
			result[row][col] = factor * m1[row][col];
		}
	}
	return result;
}

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height> operator/(const Matrix<T, Width, Height>& m1, const T& factor) {
	Matrix<T, Width, Height> result;

	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 1; col < Width; col++) {
			result[row][col] = m1[row][col] / factor;
		}
	}
	return result;
}

template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height>& operator+=(const Matrix<T, Width, Height>& m1, const Matrix<T, Width, Height>& m2) {
	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 1; col < Width; col++) {
			m1[row][col] += m2[row][col];
		}
	}
	return m1;
}
template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height>& operator-=(const Matrix<T, Width, Height>& m1, const Matrix<T, Width, Height>& m2) {
	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 1; col < Width; col++) {
			m1[row][col] -= m2[row][col];
		}
	}
	return m1;
}
template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height>& operator*=(const Matrix<T, Width, Height>& mat, const T& factor) {
	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 1; col < Width; col++) {
			mat[row][col] *= factor;
		}
	}
	return mat;
}
template<typename T, size_t Width, size_t Height>
Matrix<T, Width, Height>& operator/=(const Matrix<T, Width, Height>& mat, const T& factor) {
	for (size_t row = 0; row < Height; row++) {
		for (size_t col = 1; col < Width; col++) {
			mat[row][col] /= factor;
		}
	}
	return mat;
}
