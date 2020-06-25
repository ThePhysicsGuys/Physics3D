#pragma once

#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/cframe.h"
#include "../motion.h"

#include <cmath>

template<typename T, size_t Size>
inline bool isVecValid(const Vector<T, Size>& vec) {
	for(size_t i = 0; i < Size; i++) {
		if(!std::isfinite(vec[i])) return false;
	}
	return true;
}

template<typename T, size_t Width, size_t Height>
inline bool isMatValid(const Matrix<T, Width, Height>& mat) {
	for(size_t row = 0; row < Height; row++) {
		for(size_t col = 0; col < Width; col++) {
			if(!std::isfinite(mat[row][col])) return false;
		}
	}
	return true;
}

template<typename T, size_t Size>
inline bool isMatValid(const SymmetricMatrix<T, Size>& mat) {
	for(size_t row = 0; row < Size; row++) {
		for(size_t col = row; col < Size; col++) {
			if(!std::isfinite(mat[row][col])) return false;
		}
	}
	return true;
}

template<typename T, size_t Size>
inline bool isMatValid(const DiagonalMatrix<T, Size>& mat) {
	for(size_t i = 0; i < Size; i++) {
		if(!std::isfinite(mat[i])) return false;
	}
	return true;
}

template<typename T>
inline bool isRotationValid(const MatrixRotationTemplate<T>& rotation) {
	SquareMatrix<T, 3> rotMat = rotation.asRotationMatrix();
	return isValidRotationMatrix(rotMat);
}

template<typename T>
inline bool isRotationValid(const QuaternionRotationTemplate<T>& rotation) {
	Quaternion<T> rotQuat = rotation.asRotationQuaternion();
	return isValidRotationQuaternion(rotQuat);
}

template<typename T>
inline bool isCFrameValid(const CFrameTemplate<T>& cframe) {
	return isVecValid(cframe.getPosition()) && isRotationValid(cframe.getRotation());
}

template<typename T, std::size_t DerivationCount>
inline bool isTaylorExpansionValid(const TaylorExpansion<T, DerivationCount>& taylor) {
	for(Vec3 v : taylor) {
		if(!isVecValid(v)) return false;
	}
	return true;
}

inline bool isTranslationalMotionValid(const TranslationalMotion& motion) {
	return isTaylorExpansionValid(motion.translation);
}
inline bool isRotationalMotionValid(const RotationalMotion& motion) {
	return isTaylorExpansionValid(motion.rotation);
}
inline bool isMotionValid(const Motion& motion) {
	return isTranslationalMotionValid(motion.translation) && isRotationalMotionValid(motion.rotation);
}

struct Triangle;
class TriangleMesh;
class Polyhedron;
struct IndexedShape;

bool isValid(const TriangleMesh& mesh);
bool isValid(const Polyhedron& poly);
bool isValid(const IndexedShape& shape);

bool isValidTriangle(Triangle t, int vertexCount);
