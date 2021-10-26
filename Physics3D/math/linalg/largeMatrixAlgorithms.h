#pragma once


#include "largeMatrix.h"

#include <utility>
#include <cmath>
#include <assert.h>
#include <stddef.h>

namespace P3D {
template<typename T>
static void swapRows(UnmanagedLargeVector<T>& v, std::size_t rowA, std::size_t rowB) {
	std::swap(v[rowA], v[rowB]);
}
template<typename T>
static void swapRows(UnmanagedLargeMatrix<T>& m, std::size_t rowA, std::size_t rowB) {
	for(std::size_t i = 0; i < m.w; i++) {
		std::swap(m(rowA, i), m(rowB, i));
	}
}
template<typename T, std::size_t Cols>
static void swapRows(UnmanagedHorizontalFixedMatrix<T, Cols>& m, std::size_t rowA, std::size_t rowB) {
	for(std::size_t i = 0; i < Cols; i++) {
		std::swap(m(rowA, i), m(rowB, i));
	}
}
template<typename T, std::size_t Rows>
static void swapRows(UnmanagedVerticalFixedMatrix<T, Rows>& m, std::size_t rowA, std::size_t rowB) {
	for(std::size_t i = 0; i < m.cols; i++) {
		std::swap(m(rowA, i), m(rowB, i));
	}
}

template<typename T>
static void subtractRowsFactorTimes(UnmanagedLargeVector<T>& v, std::size_t editRow, std::size_t subtractingRow, T factor) {
	v[editRow] -= v[subtractingRow] * factor;
}
template<typename T>
static void subtractRowsFactorTimes(UnmanagedLargeMatrix<T>& m, std::size_t editRow, std::size_t subtractingRow, T factor) {
	for(std::size_t i = 0; i < m.w; i++) {
		m(editRow, i) -= m(subtractingRow, i) * factor;
	}
}
template<typename T, std::size_t Cols>
static void subtractRowsFactorTimes(UnmanagedHorizontalFixedMatrix<T, Cols>& m, std::size_t editRow, std::size_t subtractingRow, T factor) {
	for(std::size_t i = 0; i < Cols; i++) {
		m(editRow, i) -= m(subtractingRow, i) * factor;
	}
}
template<typename T, std::size_t Rows>
static void subtractRowsFactorTimes(UnmanagedVerticalFixedMatrix<T, Rows>& m, std::size_t editRow, std::size_t subtractingRow, T factor) {
	for(std::size_t i = 0; i < m.cols; i++) {
		m(editRow, i) -= m(subtractingRow, i) * factor;
	}
}

template<typename T>
static void multiplyRowBy(UnmanagedLargeVector<T>& v, std::size_t row, T factor) {
	v[row] *= factor;
}
template<typename T>
static void multiplyRowBy(UnmanagedLargeMatrix<T>& m, std::size_t row, T factor) {
	for(std::size_t i = 0; i < m.w; i++) {
		m(row, i) *= factor;
	}
}
template<typename T, std::size_t Cols>
static void multiplyRowBy(UnmanagedHorizontalFixedMatrix<T, Cols>& m, std::size_t row, T factor) {
	for(std::size_t i = 0; i < Cols; i++) {
		m(row, i) *= factor;
	}
}
template<typename T, std::size_t Rows>
static void multiplyRowBy(UnmanagedVerticalFixedMatrix<T, Rows>& m, std::size_t row, T factor) {
	for(std::size_t i = 0; i < m.cols; i++) {
		m(row, i) *= factor;
	}
}

template<typename T>
static std::size_t getHeight(UnmanagedLargeVector<T>& v) {
	return v.n;
}
template<typename T>
static std::size_t getHeight(UnmanagedLargeMatrix<T>& m) {
	return m.h;
}
template<typename T, std::size_t Cols>
static std::size_t getHeight(UnmanagedHorizontalFixedMatrix<T, Cols>& m) {
	return m.rows;
}
template<typename T, std::size_t Rows>
static std::size_t getHeight(UnmanagedVerticalFixedMatrix<T, Rows>& m) {
	return Rows;
}

template<typename System, typename SolutionType>
void destructiveSolve(System& m, SolutionType& v) {
	assert(getHeight(v) == m.w && m.w == m.h);
	std::size_t size = getHeight(v);

	// make matrix upper triangular
	for(std::size_t i = 0; i < size; i++) {
		auto bestPivot = std::abs(m(i, i));
		std::size_t bestPivotIndex = i;
		for(std::size_t j = i + 1; j < size; j++) {
			auto newPivot = std::abs(m(j, i));
			if(newPivot > bestPivot) {
				bestPivot = newPivot;
				bestPivotIndex = j;
			}
		}

		if(bestPivotIndex != i) {
			swapRows(m, bestPivotIndex, i);
			swapRows(v, bestPivotIndex, i);
		}

		auto pivot = m(i, i);

		for(std::size_t j = i + 1; j < size; j++) {
			auto factor = m(j, i) / pivot;

			m(j, i) -= m(i, i) * factor;

			for(std::size_t k = i + 1; k < size; k++) {
				m(j, k) -= m(i, k) * factor;
			}
			subtractRowsFactorTimes(v, j, i, factor);
		}
	}

	// back substitution
	for(signed long long i = size - 1; i >= 0; i--) {
		multiplyRowBy(v, i, 1 / m(i, i));
		for(signed long long j = i - 1; j >= 0; j--) {
			subtractRowsFactorTimes(v, j, i, m(j, i));
		}
	}
}
};