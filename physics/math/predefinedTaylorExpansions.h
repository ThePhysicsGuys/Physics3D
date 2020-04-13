#pragma once

#include <cmath>
#include <stddef.h>

#include "taylorExpansion.h"

template<typename T, std::size_t N>
TaylorExpansion<T, N> generateTaylorForSinWave(T currentAngle, T frequencyMultiplier) {
	TaylorExpansion<T, N> result;

	T cosValue = std::cos(currentAngle * frequencyMultiplier);
	result[0] = cosValue * frequencyMultiplier;
	if constexpr(N > 1) {
		T totalMultiplier = frequencyMultiplier;
		T sinValue = std::sin(currentAngle * frequencyMultiplier);
		T values[4]{cosValue, -sinValue, -cosValue, sinValue};
		for(std::size_t i = 1; i < N; i++) {
			totalMultiplier *= frequencyMultiplier;
			result[i] = values[i % 4] * totalMultiplier;
		}
	}

	return result;
}

template<typename T, std::size_t N>
TaylorExpansion<T, N> generateTaylorForCosWave(T currentAngle, T frequencyMultiplier) {
	TaylorExpansion<T, N> result;

	T sinValue = std::sin(currentAngle * frequencyMultiplier);
	result[0] = -sinValue * frequencyMultiplier;
	if constexpr(N > 1) {
		T totalMultiplier = frequencyMultiplier;
		T cosValue = std::cos(currentAngle * frequencyMultiplier);
		T values[4]{-sinValue, -cosValue, sinValue, cosValue};
		for(std::size_t i = 1; i < N; i++) {
			totalMultiplier *= frequencyMultiplier;
			result[i] = values[i % 4] * totalMultiplier;
		}
	}

	return result;
}

template<typename T, std::size_t N>
FullTaylorExpansion<T, T, N> generateFullTaylorForSinWave(T currentAngle, T frequencyMultiplier) {
	FullTaylorExpansion<T, T, N> result;

	T sinValue = std::sin(currentAngle * frequencyMultiplier);
	T cosValue = std::cos(currentAngle * frequencyMultiplier);

	result.constantValue = sinValue;

	T values[4]{cosValue, -sinValue, -cosValue, sinValue};

	T totalMultiplier = frequencyMultiplier;
	for(std::size_t i = 0; i < N; i++) {
		result.derivatives[i] = values[i % 4] * totalMultiplier;
		totalMultiplier *= frequencyMultiplier;
	}

	return result;
}

template<typename T, std::size_t N>
FullTaylorExpansion<T, T, N> generateFullTaylorForCosWave(T currentAngle, T frequencyMultiplier) {
	FullTaylorExpansion<T, T, N> result;

	T sinValue = std::sin(currentAngle * frequencyMultiplier);
	T cosValue = std::cos(currentAngle * frequencyMultiplier);

	result.constantValue = cosValue;

	T values[4]{-sinValue, -cosValue, sinValue, cosValue};

	T totalMultiplier = frequencyMultiplier;
	for(std::size_t i = 0; i < N; i++) {
		result.derivatives[i] = values[i % 4] * totalMultiplier;
		totalMultiplier *= frequencyMultiplier;
	}

	return result;
}
