#pragma once

#include <cmath>
#include <stddef.h>

#include "taylorExpansion.h"
#include "linalg/trigonometry.h"

namespace P3D {
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
FullTaylorExpansion<T, N> generateFullTaylorForSinWave(T currentAngle, T frequencyMultiplier) {
	T sinValue = std::sin(currentAngle * frequencyMultiplier);
	T cosValue = std::cos(currentAngle * frequencyMultiplier);

	FullTaylorExpansion<T, N> result{sinValue};

	T values[4]{cosValue, -sinValue, -cosValue, sinValue};

	T totalMultiplier = frequencyMultiplier;
	for(std::size_t i = 0; i < N - 1; i++) {
		result.setDerivative(i, values[i % 4] * totalMultiplier);
		totalMultiplier *= frequencyMultiplier;
	}

	return result;
}

template<typename T, std::size_t N>
FullTaylorExpansion<T, N> generateFullTaylorForCosWave(T currentAngle, T frequencyMultiplier) {
	T sinValue = std::sin(currentAngle * frequencyMultiplier);
	T cosValue = std::cos(currentAngle * frequencyMultiplier);

	FullTaylorExpansion<T, N> result{cosValue};

	T values[4]{-sinValue, -cosValue, sinValue, cosValue};

	T totalMultiplier = frequencyMultiplier;
	for(std::size_t i = 0; i < N - 1; i++) {
		result.setDerivative(i, values[i % 4] * totalMultiplier);
		totalMultiplier *= frequencyMultiplier;
	}

	return result;
}

template<typename T, int Derivs>
TaylorExpansion<SymmetricMatrix<T, 3>, Derivs - 1> generateTaylorForSkewSymmetricSquared(const FullTaylorExpansion<Vector<T, 3>, Derivs>& inputVector) {
	static_assert(Derivs >= 2 && Derivs <= 3);

	Vector<T, 3> f = inputVector.getConstantValue();

	TaylorExpansion<SymmetricMatrix<T, 3>, Derivs - 1> result;

	if constexpr(Derivs >= 2) {
		Vector<T, 3> ff = inputVector.getDerivative(0);
		Vector<T, 3> ffxf = elementWiseMul(ff, f);
		Vector<T, 3> ffMixed = mulOppositesBiDir(ff, f);

		result[0] = SymmetricMatrix<T, 3>{
			-2 * (ffxf.y + ffxf.z),
			ffMixed.z, -2 * (ffxf.x + ffxf.z),
			ffMixed.y, ffMixed.x, -2 * (ffxf.x + ffxf.y)
		};

		if constexpr(Derivs >= 3) {
			Vector<T, 3> fff = inputVector.getDerivative(1);
			Vector<T, 3> fffxf = elementWiseSquare(ff) + elementWiseMul(fff, f);
			Vector<T, 3> fffMixed = mulOppositesBiDir(f, fff) + mulSelfOpposites(ff) * T(2);

			result[1] = SymmetricMatrix<T, 3>{
				-2 * (fffxf.y + fffxf.z),
				fffMixed.z, -2 * (fffxf.x + fffxf.z),
				fffMixed.y, fffMixed.x, -2 * (fffxf.x + fffxf.y)
			};
		}
	}

	return result;
}

template<typename T, int Derivs>
FullTaylorExpansion<SymmetricMatrix<T, 3>, Derivs> generateFullTaylorForSkewSymmetricSquared(const FullTaylorExpansion<Vector<T, 3>, Derivs>& inputVector) {
	return FullTaylorExpansion<SymmetricMatrix<T, 3>, Derivs>::fromConstantAndDerivatives(skewSymmetricSquared(inputVector.getConstantValue()), generateTaylorForSkewSymmetricSquared<double, Derivs>(inputVector));
}

template<typename T, int Derivs>
TaylorExpansion<Mat3, Derivs> generateTaylorForRotationMatrix(const TaylorExpansion<Vec3, Derivs>& rotationVecTaylor, const Matrix<T, 3, 3>& rotation) {
	Mat3 angularVelEquiv = createCrossProductEquivalent(rotationVecTaylor[0]);

	TaylorExpansion<Mat3, Derivs> result;
	result[0] = angularVelEquiv * rotation;

	if constexpr(Derivs >= 2) {
		Mat3 angularAccelEquiv = createCrossProductEquivalent(rotationVecTaylor[1]);
		result[1] = (angularAccelEquiv + angularVelEquiv * angularVelEquiv) * rotation;
	}

	// TODO add further derivatives

	return result;
}
};