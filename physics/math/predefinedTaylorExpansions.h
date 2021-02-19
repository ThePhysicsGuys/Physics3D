#pragma once

#include <cmath>
#include <stddef.h>

#include "taylorExpansion.h"
#include "linalg/trigonometry.h"

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
	for(std::size_t i = 0; i < N-1; i++) {
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
	for(std::size_t i = 0; i < N-1; i++) {
		result.setDerivative(i, values[i % 4] * totalMultiplier);
		totalMultiplier *= frequencyMultiplier;
	}

	return result;
}

/*
	Pascal indices for constructing derivatives of multiplications
		 1
		1 1
	   1 2 1
	  1 3 3 1
	 1 4 6 4 1
*/
template<int Layer, int Index>
struct PascalIndex {
	enum { value = PascalIndex<Layer - 1, Index - 1>::value + PascalIndex<Layer - 1, Index>::value };
};

template<int Index>
struct PascalIndex<0, Index> {
	enum { value = 1 };
};

template<int Layer>
struct PascalIndex<Layer, 0> {
	enum { value = 1 };
};

template<int Layer>
struct PascalIndex<Layer, Layer> {
	enum { value = 1 };
};

// get the pascal triangle index of this layer and index
// should always get optimized to a constant expression
constexpr int pascalIndex(int layer, int index) {
	if(layer == 0 || index == 0 || index == layer) {
		return 1;
	} else {
		return pascalIndex(layer - 1, index - 1) + pascalIndex(layer - 1, index);
	}
}

template<typename T1, typename T2, std::size_t Size, std::size_t Layer, std::size_t Index>
auto computeDerivativeForIndex(const Derivatives<T1, Size>& first, const Derivatives<T2, Size>& second) -> decltype(first[0] * second[0]) {
	decltype(first[0] * second[0]) thisStepResult = first[Index] * second[Layer - Index];
	if constexpr(Index == 0) {
		return thisStepResult;
	} else if constexpr(Index == Layer) {
		return thisStepResult + computeDerivativeForIndex<T1, T2, Size, Layer, Index - 1>(first, second);
	} else {
		return static_cast<int>(PascalIndex<Layer, Index>::value) * thisStepResult + computeDerivativeForIndex<T1, T2, Size, Layer, Index - 1>(first, second);
	}
}

template<typename T1, typename T2, std::size_t Size, std::size_t CurDerivative>
void computeDerivatives(const Derivatives<T1, Size>& first, const Derivatives<T2, Size>& second, Derivatives<decltype(first[0] * second[0]), Size>& result) {
	if constexpr(CurDerivative > 0) {
		computeDerivatives<T1, T2, Size, CurDerivative - 1>(first, second, result);
	}
	result[CurDerivative] = computeDerivativeForIndex<T1, T2, Size, CurDerivative, CurDerivative>(first, second);
}

// multiplication-like derivatives
template<typename T1, typename T2, std::size_t Size>
auto derivsOfMultiplication(const Derivatives<T1, Size>& first, const Derivatives<T2, Size>& second) -> Derivatives<decltype(first[0] * second[0]), Size> {
	Derivatives<decltype(first[0] * second[0]), Size> result;

	computeDerivatives<T1, T2, Size, Size - 1>(first, second, result);

	return result;
}
// multiplication-like derivatives
template<typename T1, typename T2, std::size_t Size>
auto derivsOfMultiplication(const FullTaylorExpansion<T1, Size>& first, const FullTaylorExpansion<T2, Size>& second) -> FullTaylorExpansion<decltype(first.derivs[0] * second.derivs[0]), Size> {
	return FullTaylorExpansion<decltype(first.derivs[0] * second.derivs[0]), Size>{derivsOfMultiplication(first.derivs, second.derivs)};
}



template<typename T, int Derivs>
TaylorExpansion<SymmetricMatrix<T, 3>, Derivs - 1> generateTaylorForSkewSymmetricSquared(const FullTaylorExpansion<Vector<T, 3>, Derivs> & inputVector) {
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

// the result still has to be multiplied by the rotation (result * rotation) == realResult, but since the rotation factors out so nicely, it is left out
template<typename T, int Derivs>
TaylorExpansion<Mat3, Derivs> generateRotationTaylor(const TaylorExpansion<Vec3, Derivs>& rotationVecTaylor) {
	Mat3 angularVelEquiv = createCrossProductEquivalent(rotationVecTaylor[0]);

	TaylorExpansion<Mat3, Derivs> result;
	result[0] = angularVelEquiv;

	if constexpr(Derivs >= 2) {
		Mat3 angularAccelEquiv = createCrossProductEquivalent(rotationVecTaylor[1]);
		result[1] = angularAccelEquiv + angularVelEquiv * angularVelEquiv;

		if constexpr(Derivs >= 3) {
			Mat3 angularJerkEquiv = createCrossProductEquivalent(rotationVecTaylor[2]);
			Mat3 velAccel = angularVelEquiv * angularAccelEquiv;
			result[2] = angularJerkEquiv + (velAccel + velAccel) + result[1] * angularVelEquiv;
		}
	}

	// extending this function with more derivatives must always keep in mind the implicit rotation
	// result[i] * rotation
	// and deriving rotation yields: rotation' = angularVelEquiv * rotation
	// so this gives result[i+1]*rotation == (result[i]*rotation)' == (result[i]' * rotation + result[i] * rotation') == (result[i]' * rotation + result[i] * angularVelEquiv * rotation) == (result[i]' + result[i] * angularVelEquiv) * rotation

	return result;
}

template<typename T, int Derivs>
TaylorExpansion<Mat3, Derivs> generateTaylorForRotationMatrix(const TaylorExpansion<Vec3, Derivs>& rotationVecTaylor, const Matrix<T, 3, 3> & rotation) {
	return generateRotationTaylor<double, Derivs>(rotationVecTaylor) * rotation;
}


