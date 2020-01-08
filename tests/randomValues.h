#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <random>

#include "../physics/math/linalg/vec.h"
#include "../physics/math/linalg/mat.h"
#include "../physics/math/cframe.h"

#include "../physics/math/linalg/trigonometry.h"

// creates a random vector with elements between -1.0 and 1.0
template<typename T, size_t Size>
Vector<T, Size> createRandomVec() {
	Vector<T, Size> result;
	for(size_t i = 0; i < Size; i++) {
		result[i] = 2.0 * rand() / RAND_MAX - 1.0;
	}
	return result;
}
// creates a random average sized vector with elements between -1.0 and 1.0
template<typename T, size_t Size>
Vector<T, Size> createRandomNonzeroVec() {
	Vector<T, Size> result;
	tryAgain:
	for(size_t i = 0; i < Size; i++) {
		result[i] = 2.0 * rand() / RAND_MAX - 1.0;
	}
	if(lengthSquared(result) < 0.3) {
		goto tryAgain;
	}
	return result;
}
// creates a random rotation
template<typename T>
Matrix<T, 3, 3> createRandomRotation() {
	Vector<T, 3> angles = createRandomVec<T, 3>() * (M_PI / 2);
	return fromEulerAngles(angles.x, angles.y, angles.z);
}
// creates a random rotation
template<typename T>
CFrameTemplate<T> createRandomCFrame() {
	return CFrameTemplate<T>(createRandomNonzeroVec<double, 3>(), createRandomRotation<double>());
}
