#pragma once

#include <cmath>
#include <stddef.h>

#include "taylorExpansion.h"

template<typename T, std::size_t N>
TaylorExpansion<T, N> forSinWave(T currentAngle) {
	TaylorExpansion<T, N> result;

	T cosValue = std::cos(currentAngle);
	result[0] = cosValue;
	if constexpr(N > 1) {
		T sinValue = std::sin(currentAngle);
		T values[4]{cosValue, -sinValue, -cosValue, sinValue};
		for(std::size_t i = 1; i < N; i++) {
			result[i] = values[i % 4];
		}
	}

	return result;
}

template<typename T, std::size_t N>
TaylorExpansion<T, N> forCosWave(T currentAngle) {
	TaylorExpansion<T, N> result;

	T sinValue = std::sin(currentAngle);
	result[0] = -sinValue;
	if constexpr(N > 1) {
		T cosValue = std::cos(currentAngle);
		T values[4]{-sinValue, -cosValue, sinValue, cosValue};
		for(std::size_t i = 1; i < N; i++) {
			result[i] = values[i % 4];
		}
	}

	return result;
}
