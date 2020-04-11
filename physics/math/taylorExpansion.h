#pragma once

#include <stddef.h>

template<typename T, std::size_t DerivationCount>
struct TaylorExpansion {
	T derivatives[DerivationCount] = {};

	constexpr const T& operator[](std::size_t index) const { return derivatives[index]; }
	constexpr T& operator[](std::size_t index) { return derivatives[index]; }

	T* begin() { return derivatives; }
	const T* begin() const { return derivatives; }
	T* end() { return derivatives + DerivationCount; }
	const T* end() const { return derivatives + DerivationCount; }

	/*
		Evaluates the taylor expansion at x
		returns (derivatives[0]+(derivatives[1]+(derivatives[2]+...)*(x/3))*(x/2))*x
		= derivatives[0]*x + derivatives[1]*x^2/2! + derivatives[2]*x^3/3! + derivatives[3]*x^4/4! + ...
	*/
	template<typename T2>
	auto operator()(const T2& x) const -> decltype(derivatives[0] * x) {
		decltype(derivatives[0] * x) result = derivatives[DerivationCount - 1];
		
		for(std::size_t i = DerivationCount - 1; i > 0; i--) {
			result *= x / (i+1);
			result += derivatives[i-1];
		}
		result *= x;
		return result;
	}

	constexpr std::size_t size() const { return DerivationCount; }
};

template<typename T, std::size_t DerivationCount>
auto operator-(const TaylorExpansion<T, DerivationCount>& obj) -> TaylorExpansion<decltype(-obj.derivatives[0]), DerivationCount> {
	TaylorExpansion<decltype(-obj.derivatives[0]), DerivationCount> result;

	for(std::size_t i = 0; i < DerivationCount; i++) {
		result.derivatives[i] = -obj.derivatives[i];
	}

	return result;
}

template<typename T1, typename T2, std::size_t DerivationCount>
auto operator+(const TaylorExpansion<T1, DerivationCount>& first, const TaylorExpansion<T2, DerivationCount>& second) -> TaylorExpansion<decltype(first.derivatives[0] + second.derivatives[0]), DerivationCount> {
	TaylorExpansion<decltype(first.derivatives[0] + second.derivatives[0]), DerivationCount> result;

	for(std::size_t i = 0; i < DerivationCount; i++) {
		result.derivatives[i] = first.derivatives[i] + second.derivatives[i];
	}

	return result;
}
template<typename T1, typename T2, std::size_t DerivationCount>
auto operator-(const TaylorExpansion<T1, DerivationCount>& first, const TaylorExpansion<T2, DerivationCount>& second) -> TaylorExpansion<decltype(first.derivatives[0] - second.derivatives[0]), DerivationCount> {
	TaylorExpansion<decltype(first.derivatives[0] - second.derivatives[0]), DerivationCount> result;

	for(std::size_t i = 0; i < DerivationCount; i++) {
		result.derivatives[i] = first.derivatives[i] - second.derivatives[i];
	}

	return result;
}
template<typename T1, typename T2, std::size_t DerivationCount>
auto operator*(const T1& factor, const TaylorExpansion<T2, DerivationCount>& second) -> TaylorExpansion<decltype(factor * second.derivatives[0]), DerivationCount> {
	TaylorExpansion<decltype(factor * second.derivatives[0]), DerivationCount> result;

	for(std::size_t i = 0; i < DerivationCount; i++) {
		result.derivatives[i] = factor * second.derivatives[i];
	}

	return result;
}
template<typename T1, typename T2, std::size_t DerivationCount>
auto operator*(const TaylorExpansion<T1, DerivationCount>& first, const T2& factor) -> TaylorExpansion<decltype(first.derivatives[0] * factor), DerivationCount> {
	TaylorExpansion<decltype(first.derivatives[0] * factor), DerivationCount> result;

	for(std::size_t i = 0; i < DerivationCount; i++) {
		result.derivatives[i] = first.derivatives[i] * factor;
	}

	return result;
}
template<typename T1, typename T2, std::size_t DerivationCount>
auto operator/(const TaylorExpansion<T1, DerivationCount>& first, const T2& factor) -> TaylorExpansion<decltype(first.derivatives[0] / factor), DerivationCount> {
	TaylorExpansion<decltype(first.derivatives[0] / factor), DerivationCount> result;

	for(std::size_t i = 0; i < DerivationCount; i++) {
		result.derivatives[i] = first.derivatives[i] / factor;
	}

	return result;
}
template<typename T1, typename T2, std::size_t DerivationCount>
auto operator%(const TaylorExpansion<T1, DerivationCount>& first, const T2& factor) -> TaylorExpansion<decltype(first.derivatives[0] % factor), DerivationCount> {
	TaylorExpansion<decltype(first.derivatives[0] % factor), DerivationCount> result;

	for(std::size_t i = 0; i < DerivationCount; i++) {
		result.derivatives[i] = first.derivatives[i] % factor;
	}

	return result;
}

template<typename T1, typename T2, std::size_t DerivationCount>
TaylorExpansion<T1, DerivationCount>& operator+=(TaylorExpansion<T1, DerivationCount>& first, const TaylorExpansion<T2, DerivationCount>& second) {
	for(std::size_t i = 0; i < DerivationCount; i++) {
		first.derivatives[i] += second.derivatives[i];
	}

	return first;
}
template<typename T1, typename T2, std::size_t DerivationCount>
TaylorExpansion<T1, DerivationCount>& operator-=(TaylorExpansion<T1, DerivationCount>& first, const TaylorExpansion<T2, DerivationCount>& second) {
	for(std::size_t i = 0; i < DerivationCount; i++) {
		first.derivatives[i] -= second.derivatives[i];
	}

	return first;
}
template<typename T1, typename T2, std::size_t DerivationCount>
TaylorExpansion<T1, DerivationCount>& operator*=(TaylorExpansion<T1, DerivationCount>& first, const T2& factor) {
	for(std::size_t i = 0; i < DerivationCount; i++) {
		first.derivatives[i] *= factor;
	}

	return first;
}
template<typename T1, typename T2, std::size_t DerivationCount>
TaylorExpansion<T1, DerivationCount>& operator/=(TaylorExpansion<T1, DerivationCount>& first, const T2& factor) {
	for(std::size_t i = 0; i < DerivationCount; i++) {
		first.derivatives[i] /= factor;
	}

	return first;
}
template<typename T1, typename T2, std::size_t DerivationCount>
TaylorExpansion<T1, DerivationCount>& operator%=(TaylorExpansion<T1, DerivationCount>& first, const T2& factor) {
	for(std::size_t i = 0; i < DerivationCount; i++) {
		first.derivatives[i] %= factor;
	}

	return first;
}

