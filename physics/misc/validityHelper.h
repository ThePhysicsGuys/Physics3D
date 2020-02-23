#pragma once

#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/cframe.h"
#include "../motion.h"

#include <cmath>

template<typename T, size_t Size>
inline static bool isVecValid(const Vector<T, Size>& vec) {
	for(size_t i = 0; i < Size; i++) {
		if(!isfinite(vec[i])) return false;
	}
	return true;
}

template<typename T, size_t Width, size_t Height>
inline static bool isMatValid(const Matrix<T, Width, Height>& mat) {
	for(size_t row = 0; row < Height; row++) {
		for(size_t col = 0; col < Width; col++) {
			if(!isfinite(mat[row][col])) return false;
		}
	}
	return true;
}

template<typename T, size_t Size>
inline static bool isMatValid(const SymmetricMatrix<T, Size>& mat) {
	for(size_t row = 0; row < Size; row++) {
		for(size_t col = row; col < Size; col++) {
			if(!isfinite(mat[row][col])) return false;
		}
	}
	return true;
}

template<typename T, size_t Size>
inline static bool isMatValid(const DiagonalMatrix<T, Size>& mat) {
	for(size_t i = 0; i < Size; i++) {
		if(!isfinite(mat[i])) return false;
	}
	return true;
}

template<typename T>
inline static bool isCFrameValid(const CFrameTemplate<T>& cframe) {
	return isVecValid(cframe.getPosition()) && isMatValid(cframe.getRotation().asRotationMatrix()) && abs(det(cframe.getRotation().asRotationMatrix()) - 1.0) < 0.00002;
}

inline static bool isTranslationalMotionValid(const TranslationalMotion& motion) {
	return isVecValid(motion.velocity) && isVecValid(motion.acceleration);
}
inline static bool isRotationalMotionValid(const RotationalMotion& motion) {
	return isVecValid(motion.angularVelocity) && isVecValid(motion.angularAcceleration);
}


inline static bool isMotionValid(const Motion& motion) {
	return isTranslationalMotionValid(motion.translation) && isRotationalMotionValid(motion.rotation);
}