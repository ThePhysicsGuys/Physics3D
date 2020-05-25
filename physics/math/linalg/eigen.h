#pragma once

#include "mat.h"
#include <initializer_list>
#include <stddef.h>

template<typename T, std::size_t Size>
struct EigenValues {
	T values[Size];

	EigenValues(std::initializer_list<T> list) {
		assert(list.size() == Size);
		const double* listValues = list.begin();
		for(std::size_t i = 0; i < Size; i++) {
			values[i] = listValues[i];
		}
	}

	DiagonalMatrix<T, Size> asDiagonalMatrix() const {
		return DiagonalMatrix<T, Size>(values);
	}

	const T& operator[](std::size_t index) const { return values[index]; }
	T& operator[](std::size_t index) { return values[index]; }
};

template<typename T, std::size_t Size>
struct EigenSet {
	EigenValues<T, Size> eigenValues;
	Matrix<T, Size, Size> eigenVectors;
	EigenSet(EigenValues<T, Size> eigenValues, const Matrix<T, Size, Size>& eigenVectors) : eigenValues(eigenValues), eigenVectors(eigenVectors) {};
};

EigenSet<double, 3> getEigenDecomposition(const SymmetricMat3& sm);
