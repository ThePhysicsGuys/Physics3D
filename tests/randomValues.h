#pragma once

#include <stddef.h>

#include <random>
#include <array>

#include <Physics3D/math/linalg/vec.h>
#include <Physics3D/math/linalg/mat.h>
#include <Physics3D/math/cframe.h>
#include <Physics3D/math/constants.h>
#include <Physics3D/math/linalg/trigonometry.h>

#include <Physics3D/motion.h>
#include <Physics3D/relativeMotion.h>

namespace P3D {
inline double createRandomDouble() {
	return 2.0 * rand() / RAND_MAX - 1.0;
}

inline double createRandomNonzeroDouble() {
	tryAgain:
	double result = createRandomDouble();

	if(std::abs(result) < 0.3) {
		goto tryAgain;
	}

	return result;
}

// creates a random vector with elements between -1.0 and 1.0
template<typename T, std::size_t Size>
Vector<T, Size> createRandomVecTemplate() {
	Vector<T, Size> result;
	for(std::size_t i = 0; i < Size; i++) {
		result[i] = createRandomDouble();
	}
	return result;
}
// creates a random vector with elements between -1.0 and 1.0
template<typename T, std::size_t Height, std::size_t Width>
Matrix<T, Height, Width> createRandomMatrixTemplate() {
	Matrix<T, Height, Width> result;
	for(std::size_t y = 0; y < Height; y++) {
		for(std::size_t x = 0; x < Width; x++) {
			result(x, y) = createRandomDouble();
		}
	}
	return result;
}
// creates a random average sized vector with elements between -1.0 and 1.0
template<typename T, std::size_t Size>
Vector<T, Size> createRandomNonzeroVecTemplate() {
	Vector<T, Size> result;
	tryAgain:
	for(std::size_t i = 0; i < Size; i++) {
		result[i] = createRandomDouble();
	}
	if(lengthSquared(result) < 0.3) {
		goto tryAgain;
	}
	return result;
}
// creates a random rotation
template<typename T>
RotationTemplate<T> createRandomRotationTemplate() {
	Vector<T, 3> angles = createRandomNonzeroVecTemplate<T, 3>() * (PI / 2);
	return Rotation::fromEulerAngles(angles.x, angles.y, angles.z);
}
// creates a random rotation
template<typename T>
CFrameTemplate<T> createRandomCFrameTemplate() {
	return CFrameTemplate<T>(createRandomNonzeroVecTemplate<double, 3>(), createRandomRotationTemplate<double>());
}

inline Vec3 createRandomVec() {
	return createRandomVecTemplate<double, 3>();
}
inline Vec3 createRandomNonzeroVec3() {
	return createRandomNonzeroVecTemplate<double, 3>();
}
inline Rotation createRandomRotation() {
	return createRandomRotationTemplate<double>();
}
inline CFrame createRandomCFrame() {
	return createRandomCFrameTemplate<double>();
}

inline TranslationalMotion createRandomTranslationalMotion() {
	return TranslationalMotion(createRandomNonzeroVec3(), createRandomNonzeroVec3());
}
inline RotationalMotion createRandomRotationalMotion() {
	return RotationalMotion(createRandomNonzeroVec3(), createRandomNonzeroVec3());
}

inline Motion createRandomMotion() {
	return Motion(createRandomTranslationalMotion(), createRandomRotationalMotion());
}
inline RelativeMotion createRandomRelativeMotion() {
	return RelativeMotion(createRandomMotion(), createRandomCFrame());
}

template<typename T, std::size_t Size, T(*createFunc)()>
inline Derivatives<T, Size> createRandomDerivatives() {
	Derivatives<T, Size> result;

	for(T& item : result) {
		item = createFunc();
	}

	return result;
}
template<typename T, std::size_t Size, T(*createFunc)()>
inline TaylorExpansion<T, Size> createRandomTaylorExpansion() {
	return TaylorExpansion<T, Size>{createRandomDerivatives<T, Size, createFunc>()};
}
template<typename T, std::size_t Size, T(*createFunc)()>
inline FullTaylorExpansion<T, Size> createRandomFullTaylorExpansion() {
	return FullTaylorExpansion<T, Size>{createRandomDerivatives<T, Size, createFunc>()};
}
template<typename T, std::size_t Size, T(*createFunc)()>
inline std::array<T, Size> createRandomArray() {
	std::array<T, Size> result;

	for(T& item : result) {
		item = createFunc();
	}

	return result;
}
};
