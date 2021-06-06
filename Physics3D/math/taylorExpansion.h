#pragma once

#include <assert.h>
#include <stddef.h>
#include <initializer_list>

namespace P3D {
template<typename T, std::size_t DerivationCount>
struct Derivatives {
	T data[DerivationCount];

	constexpr const T& operator[](std::size_t index) const { assert(index >= 0 && index < DerivationCount); return data[index]; }
	constexpr T& operator[](std::size_t index) { assert(index >= 0 && index < DerivationCount); return data[index]; }

	T* begin() { return data; }
	const T* begin() const { return data; }
	T* end() { return data + DerivationCount; }
	const T* end() const { return data + DerivationCount; }

	template<typename Func>
	auto transform(const Func& transformFunc) const -> Derivatives<decltype(transformFunc(this->data[0])), DerivationCount> {
		Derivatives<decltype(transformFunc(this->data[0])), DerivationCount> result;
		for(std::size_t i = 0; i < DerivationCount; i++) {
			result[i] = transformFunc(this->data[i]);
		}
		return result;
	}
};

template<typename T, std::size_t DerivationCount>
struct TaylorExpansion {
	Derivatives<T, DerivationCount> derivs;

	constexpr const T& operator[](std::size_t index) const { return derivs[index]; }
	constexpr T& operator[](std::size_t index) { return derivs[index]; }

	T* begin() { return derivs.begin(); }
	const T* begin() const { return derivs.begin(); }
	T* end() { return derivs.end(); }
	const T* end() const { return derivs.end(); }

	/*
		Evaluates the taylor expansion at x
		returns (derivatives[0]+(derivatives[1]+(derivatives[2]+...)*(x/3))*(x/2))*x
		= derivatives[0]*x + derivatives[1]*x^2/2! + derivatives[2]*x^3/3! + derivatives[3]*x^4/4! + ...
	*/
	template<typename T2>
	T operator()(const T2& x) const {
		T result = derivs[DerivationCount - 1];
		
		for(std::size_t i = DerivationCount - 1; i > 0; i--) {
			result *= x / (i+1);
			result += derivs[i-1];
		}
		result *= x;
		return result;
	}

	template<typename Func>
	auto transform(const Func& transformFunc) const -> TaylorExpansion<decltype(transformFunc(this->derivs[0])), DerivationCount> {
		return TaylorExpansion<decltype(transformFunc(this->derivs[0])), DerivationCount>{this->derivs.transform(transformFunc)};
	}
};

template<typename T, std::size_t DerivationCount>
struct FullTaylorExpansion {
	Derivatives<T, DerivationCount> derivs;

	inline const T& getConstantValue() const { return derivs[0]; }
	inline const T& getDerivative(std::size_t i) const { return derivs[i+1]; }

	inline void setConstantValue(const T& newConstValue) { derivs[0] = newConstValue; }
	inline void setDerivative(std::size_t i, const T& newDerivative) { derivs[i+1] = newDerivative; }

	inline TaylorExpansion<T, DerivationCount-1> getDerivatives() const {
		TaylorExpansion<T, DerivationCount - 1> result;
		for(std::size_t i = 0; i < DerivationCount - 1; i++) {
			result[i] = derivs[i+1];
		}
		return result; 
	}

	static FullTaylorExpansion<T, DerivationCount> fromConstantAndDerivatives(const T& constantValue, const TaylorExpansion<T, DerivationCount-1>& newDerivs) {
		FullTaylorExpansion<T, DerivationCount> result;
		result.setConstantValue(constantValue);
		for(std::size_t i = 0; i < DerivationCount - 1; i++) {
			result.setDerivative(i, newDerivs[i]);
		}
		return result;
	}

	/*
		Evaluates the taylor expansion at x
		returns constantValue + (derivatives[0]+(derivatives[1]+(derivatives[2]+...)*(x/3))*(x/2))*x
		= constantValue + derivatives[0]*x + derivatives[1]*x^2/2! + derivatives[2]*x^3/3! + derivatives[3]*x^4/4! + ...
	*/
	template<typename T2>
	T operator()(const T2& x) const {
		std::size_t NumberOfDerivs = DerivationCount - 1;
		T derivsFactor = this->getDerivative(NumberOfDerivs - 1);

		for(std::size_t i = NumberOfDerivs - 1; i > 0; i--) {
			derivsFactor *= x / (i + 1);
			derivsFactor += this->getDerivative(i - 1);
		}
		derivsFactor *= x;

		return this->getConstantValue() + derivsFactor;
	}

	template<typename Func>
	auto transform(const Func& transformFunc) const -> FullTaylorExpansion<decltype(transformFunc(this->derivs[0])), DerivationCount> {
		return FullTaylorExpansion<decltype(transformFunc(this->derivs[0])), DerivationCount>{this->derivs.transform(transformFunc)};
	}
};



template<typename T, std::size_t DerivationCount>
Derivatives<T, DerivationCount> operator-(const Derivatives<T, DerivationCount>& obj) {
	Derivatives<T, DerivationCount> result;

	for(std::size_t i = 0; i < DerivationCount; i++) {
		result[i] = -obj[i];
	}

	return result;
}

template<typename T, std::size_t DerivationCount>
Derivatives<T, DerivationCount> operator+(const Derivatives<T, DerivationCount>& first, const Derivatives<T, DerivationCount>& second) {
	Derivatives<T, DerivationCount> result;

	for(std::size_t i = 0; i < DerivationCount; i++) {
		result[i] = first[i] + second[i];
	}

	return result;
}
template<typename T, std::size_t DerivationCount>
Derivatives<T, DerivationCount> operator-(const Derivatives<T, DerivationCount>& first, const Derivatives<T, DerivationCount>& second) {
	Derivatives<T, DerivationCount> result;

	for(std::size_t i = 0; i < DerivationCount; i++) {
		result[i] = first[i] - second[i];
	}

	return result;
}
template<typename T, typename T2, std::size_t DerivationCount>
Derivatives<T, DerivationCount> operator*(const T2& factor, const Derivatives<T, DerivationCount>& second) {
	Derivatives<T, DerivationCount> result;

	for(std::size_t i = 0; i < DerivationCount; i++) {
		result[i] = factor * second[i];
	}

	return result;
}
template<typename T, typename T2, std::size_t DerivationCount>
Derivatives<T, DerivationCount> operator*(const Derivatives<T, DerivationCount>& first, const T2& factor) {
	Derivatives<T, DerivationCount> result;

	for(std::size_t i = 0; i < DerivationCount; i++) {
		result[i] = first[i] * factor;
	}

	return result;
}
template<typename T, typename T2, std::size_t DerivationCount>
Derivatives<T, DerivationCount> operator/(const Derivatives<T, DerivationCount>& first, const T2& factor) {
	Derivatives<T, DerivationCount> result;

	for(std::size_t i = 0; i < DerivationCount; i++) {
		result[i] = first[i] / factor;
	}

	return result;
}

template<typename T, std::size_t DerivationCount>
Derivatives<T, DerivationCount>& operator+=(Derivatives<T, DerivationCount>& first, const Derivatives<T, DerivationCount>& second) {
	for(std::size_t i = 0; i < DerivationCount; i++) {
		first[i] += second[i];
	}

	return first;
}
template<typename T, std::size_t DerivationCount>
Derivatives<T, DerivationCount>& operator-=(Derivatives<T, DerivationCount>& first, const Derivatives<T, DerivationCount>& second) {
	for(std::size_t i = 0; i < DerivationCount; i++) {
		first[i] -= second[i];
	}

	return first;
}
template<typename T, typename T2, std::size_t DerivationCount>
Derivatives<T, DerivationCount>& operator*=(Derivatives<T, DerivationCount>& first, const T2& factor) {
	for(std::size_t i = 0; i < DerivationCount; i++) {
		first[i] *= factor;
	}

	return first;
}
template<typename T, typename T2, std::size_t DerivationCount>
Derivatives<T, DerivationCount>& operator/=(Derivatives<T, DerivationCount>& first, const T2& factor) {
	for(std::size_t i = 0; i < DerivationCount; i++) {
		first[i] /= factor;
	}

	return first;
}


template<typename T, std::size_t DerivationCount>
TaylorExpansion<T, DerivationCount> operator-(const TaylorExpansion<T, DerivationCount>& obj) {
	return TaylorExpansion<T, DerivationCount>{-obj.derivs};
}

template<typename T, std::size_t DerivationCount>
TaylorExpansion<T, DerivationCount> operator+(const TaylorExpansion<T, DerivationCount>& first, const TaylorExpansion<T, DerivationCount>& second) {
	return TaylorExpansion<T, DerivationCount>{first.derivs + second.derivs};
}

template<typename T, std::size_t DerivationCount>
TaylorExpansion<T, DerivationCount> operator-(const TaylorExpansion<T, DerivationCount>& first, const TaylorExpansion<T, DerivationCount>& second) {
	return TaylorExpansion<T, DerivationCount>{first.derivs - second.derivs};
}

template<typename T, typename T2, std::size_t DerivationCount>
TaylorExpansion<T, DerivationCount> operator*(const TaylorExpansion<T, DerivationCount>& first, const T2& second) {
	return TaylorExpansion<T, DerivationCount>{first.derivs * second};
}

template<typename T, typename T2, std::size_t DerivationCount>
TaylorExpansion<T, DerivationCount> operator*(const T2& first, const TaylorExpansion<T, DerivationCount>& second) {
	return TaylorExpansion<T, DerivationCount>{first * second.derivs};
}

template<typename T, typename T2, std::size_t DerivationCount>
TaylorExpansion<T, DerivationCount> operator/(const TaylorExpansion<T, DerivationCount>& first, const T2& second) {
	return TaylorExpansion<T, DerivationCount>{first.derivs / second};
}

template<typename T, std::size_t DerivationCount>
TaylorExpansion<T, DerivationCount>& operator+=(TaylorExpansion<T, DerivationCount>& first, const TaylorExpansion<T, DerivationCount>& second) {
	first.derivs += second.derivs;
	return first;
}

template<typename T, std::size_t DerivationCount>
TaylorExpansion<T, DerivationCount>& operator-=(TaylorExpansion<T, DerivationCount>& first, const TaylorExpansion<T, DerivationCount>& second) {
	first.derivs -= second.derivs;
	return first;
}

template<typename T, typename T2, std::size_t DerivationCount>
TaylorExpansion<T, DerivationCount>& operator*=(TaylorExpansion<T, DerivationCount>& first, const T2& second) {
	first.derivs *= second;
	return first;
}

template<typename T, typename T2, std::size_t DerivationCount>
TaylorExpansion<T, DerivationCount>& operator/=(TaylorExpansion<T, DerivationCount>& first, const T2& second) {
	first.derivs /= second;
	return first;
}



template<typename T, std::size_t DerivationCount>
FullTaylorExpansion<T, DerivationCount> operator-(const FullTaylorExpansion<T, DerivationCount>& obj) {
	return FullTaylorExpansion<T, DerivationCount>{-obj.derivs};
}

template<typename T, std::size_t DerivationCount>
FullTaylorExpansion<T, DerivationCount> operator+(const FullTaylorExpansion<T, DerivationCount>& first, const FullTaylorExpansion<T, DerivationCount>& second) {
	return FullTaylorExpansion<T, DerivationCount>{first.derivs + second.derivs};
}

template<typename T, std::size_t DerivationCount>
FullTaylorExpansion<T, DerivationCount> operator-(const FullTaylorExpansion<T, DerivationCount>& first, const FullTaylorExpansion<T, DerivationCount>& second) {
	return FullTaylorExpansion<T, DerivationCount>{first.derivs - second.derivs};
}

template<typename T, std::size_t DerivationCount>
FullTaylorExpansion<T, DerivationCount> operator+(const FullTaylorExpansion<T, DerivationCount>& first, const T& second) {
	FullTaylorExpansion<T, DerivationCount> result = first;
	result.derivs[0] += second;
	return result;
}

template<typename T, std::size_t DerivationCount>
FullTaylorExpansion<T, DerivationCount> operator-(const FullTaylorExpansion<T, DerivationCount>& first, const T& second) {
	FullTaylorExpansion<T, DerivationCount> result = first;
	result.derivs[0] -= second;
	return result;
}

template<typename T, typename T2, std::size_t DerivationCount>
FullTaylorExpansion<T, DerivationCount> operator*(const FullTaylorExpansion<T, DerivationCount>& first, const T2& second) {
	return FullTaylorExpansion<T, DerivationCount>{first.derivs * second};
}

template<typename T, typename T2, std::size_t DerivationCount>
FullTaylorExpansion<T, DerivationCount> operator*(const T2& first, const FullTaylorExpansion<T, DerivationCount>& second) {
	return FullTaylorExpansion<T, DerivationCount>{first * second.derivs};
}

template<typename T, typename T2, std::size_t DerivationCount>
FullTaylorExpansion<T, DerivationCount> operator/(const FullTaylorExpansion<T, DerivationCount>& first, const T2& second) {
	return FullTaylorExpansion<T, DerivationCount>{first.derivs / second};
}

template<typename T, std::size_t DerivationCount>
FullTaylorExpansion<T, DerivationCount>& operator+=(FullTaylorExpansion<T, DerivationCount>& first, const FullTaylorExpansion<T, DerivationCount>& second) {
	first.derivs += second.derivs;
	return first;
}

template<typename T, std::size_t DerivationCount>
FullTaylorExpansion<T, DerivationCount>& operator-=(FullTaylorExpansion<T, DerivationCount>& first, const FullTaylorExpansion<T, DerivationCount>& second) {
	first.derivs -= second.derivs;
	return first;
}

template<typename T, std::size_t DerivationCount>
FullTaylorExpansion<T, DerivationCount>& operator+=(FullTaylorExpansion<T, DerivationCount>& first, const T& second) {
	first.derivs[0] += second;
	return first;
}

template<typename T, std::size_t DerivationCount>
FullTaylorExpansion<T, DerivationCount>& operator-=(FullTaylorExpansion<T, DerivationCount>& first, const T& second) {
	first.derivs[0] -= second;
	return first;
}

template<typename T, typename T2, std::size_t DerivationCount>
FullTaylorExpansion<T, DerivationCount>& operator*=(FullTaylorExpansion<T, DerivationCount>& first, const T2& second) {
	first.derivs *= second;
	return first;
}

template<typename T, typename T2, std::size_t DerivationCount>
FullTaylorExpansion<T, DerivationCount>& operator/=(FullTaylorExpansion<T, DerivationCount>& first, const T2& second) {
	first.derivs /= second;
	return first;
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
	enum { value = PascalIndex<Layer-1,Index-1>::value + PascalIndex<Layer - 1, Index>::value};
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
	if (layer == 0 || index == 0 || index == layer) {
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

#define DEFAULT_TAYLOR_LENGTH 2

template<typename T>
using Taylor = TaylorExpansion<T, DEFAULT_TAYLOR_LENGTH>;

template<typename T>
using FullTaylor = FullTaylorExpansion<T, DEFAULT_TAYLOR_LENGTH + 1>;
};