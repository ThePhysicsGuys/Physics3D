#pragma once

#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/cframe.h"
#include "../motion.h"

#include <cmath>

template<typename T, size_t Size>
inline static bool isVecValid(const Vector<T, Size>& vec) {
	for(size_t i = 0; i < Size; i++) {
		if(!std::isfinite(vec[i])) return false;
	}
	return true;
}

template<typename T, size_t Width, size_t Height>
inline static bool isMatValid(const Matrix<T, Width, Height>& mat) {
	for(size_t row = 0; row < Height; row++) {
		for(size_t col = 0; col < Width; col++) {
			if(!std::isfinite(mat[row][col])) return false;
		}
	}
	return true;
}

template<typename T, size_t Size>
inline static bool isMatValid(const SymmetricMatrix<T, Size>& mat) {
	for(size_t row = 0; row < Size; row++) {
		for(size_t col = row; col < Size; col++) {
			if(!std::isfinite(mat[row][col])) return false;
		}
	}
	return true;
}

template<typename T, size_t Size>
inline static bool isMatValid(const DiagonalMatrix<T, Size>& mat) {
	for(size_t i = 0; i < Size; i++) {
		if(!std::isfinite(mat[i])) return false;
	}
	return true;
}

template<typename T>
inline static bool isCFrameValid(const CFrameTemplate<T>& cframe) {
	return isVecValid(cframe.getPosition()) && isMatValid(cframe.getRotation().asRotationMatrix()) && abs(det(cframe.getRotation().asRotationMatrix()) - 1.0) < 0.00002;
}

template<typename T, std::size_t DerivationCount>
inline static bool isTaylorExpansionValid(const TaylorExpansion<T, DerivationCount>& taylor) {
	for(Vec3 v : taylor) {
		if(!isVecValid(v)) return false;
	}
	return true;
}

inline static bool isTranslationalMotionValid(const TranslationalMotion& motion) {
	return isTaylorExpansionValid(motion.translation);
}
inline static bool isRotationalMotionValid(const RotationalMotion& motion) {
	return isTaylorExpansionValid(motion.rotation);
}


inline static bool isMotionValid(const Motion& motion) {
	return isTranslationalMotionValid(motion.translation) && isRotationalMotionValid(motion.rotation);
}