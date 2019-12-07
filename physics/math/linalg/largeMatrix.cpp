#include "largeMatrix.h"

#include <cmath>
#include <utility>

template<typename T>
void swapRows(LargeMatrix<T>& m, LargeVector<T>& v, size_t rowA, size_t rowB) {
	for (size_t i = 0; i < m.width; i++) {
		std::swap(m.get(rowA, i), m.get(rowB, i));
	}
	std::swap(v[rowA], v[rowB]);
}

template<typename T>
void destructiveSolve(LargeMatrix<T>& m, LargeVector<T>& v) {
	if (v.size != m.width || m.width != m.height) throw "Dimensions do not align!";
	size_t size = v.size;

	// make matrix upper triangular
	for (size_t i = 0; i < size; i++) {
		T bestPivot = std::abs(m.get(i, i));
		size_t bestPivotIndex = i;
		for (size_t j = i + 1; j < size; j++) {
			T newPivot = std::abs(m.get(j, i));
			if (newPivot > bestPivot) {
				bestPivot = newPivot;
				bestPivotIndex = j;
			}
		}
		
		if (bestPivotIndex != i) {
			swapRows(m, v, bestPivotIndex, i);
		}

		T pivot = m.get(i, i);
		T pivotVectorElement = v[i];

		for (size_t j = i + 1; j < size; j++) {
			T factor = m.get(j, i) / pivot;

			m.get(j, i) -= m.get(i, i) * factor;

			for (size_t k = i + 1; k < size; k++) {
				m.get(j, k) -= m.get(i, k) * factor;
			}
			v[j] -= pivotVectorElement * factor;
		}
	}

	// back substitution
	for (signed long long i = size - 1; i >= 0; i--) {
		T pivot = m.get(i, i);
		T pivotVectorElement = v[i];

		v[i] /= pivot;
		for (signed long long j = i - 1; j >= 0; j--) {
			v[j] -= v[i] * m.get(j, i);
		}
	}
}

template void destructiveSolve<double>(LargeMatrix<double>& m, LargeVector<double>& v);
template void destructiveSolve<float>(LargeMatrix<float>& m, LargeVector<float>& v);

