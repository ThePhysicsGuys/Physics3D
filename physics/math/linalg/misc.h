#pragma once

#include "mat.h"

template<typename T>
Matrix<T, 3, 3> createCrossProductEquivalent(const Vector<T, 3>& vec) {
	return Matrix<T, 3, 3>{
		0, vec.z, -vec.y,
		-vec.z, 0, vec.x,
		vec.y, -vec.x, 0
	};
}

template<typename N>
SymmetricMatrix<N, 3> skewSymmetricSquared(const Vector<N, 3>& v) {
	N x = v.x, y = v.y, z = v.z;
	return SymmetricMatrix<N, 3>{
		y* y + z * z,
		-x * y, x* x + z * z,
		-x * z, -y * z, x* x + y * y
	};
}

template<typename T>
SymmetricMatrix<T, 3> transformBasis(const SymmetricMatrix<T, 3>& sm, const Matrix<T, 3, 3>& rotation) {
	Matrix<T, 3, 3> r = rotation * sm * ~rotation;
	return SymmetricMatrix<T, 3>{
		r[0][0],
		r[1][0], r[1][1],
		r[2][0], r[2][1], r[2][2]
	};
}

template<typename T>
SymmetricMatrix<T, 3> transformBasis(const DiagonalMatrix<T, 3>& dm, const Matrix<T, 3, 3>& rotation) {
	Matrix<T, 3, 3> r = rotation * dm * ~rotation;
	return SymmetricMatrix<T, 3>{
			r[0][0],
			r[1][0], r[1][1],
			r[2][0], r[2][1], r[2][2]
	};
}

template<typename T>
SymmetricMatrix<T, 3> multiplyLeftRight(const SymmetricMatrix<T, 3>& sm, const Matrix<T, 3, 3>& otherMat) {
	Matrix<T, 3, 3> r = otherMat * sm * otherMat.transpose();
	return SymmetricMatrix<T, 3>{
		r[0][0],
		r[1][0], r[1][1],
		r[2][0], r[2][1], r[2][2]
	};
}

