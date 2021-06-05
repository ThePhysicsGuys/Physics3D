#pragma once

#include <cstddef>

namespace P3D {
template<typename T, std::size_t C>
struct ParallelArray {
	T values[C];

	ParallelArray() = default;

	ParallelArray(T* values) : values{} {
		for(std::size_t i = 0; i < C; i++) {
			this->values[i] = values[i];
		}
	}

	ParallelArray<T, C> operator+(const ParallelArray<T, C>& other) const {
		T newValues[C];
		for(std::size_t i = 0; i < C; i++) {
			newValues[i] = values[i] + other.values[i];
		}
		return ParallelArray<T, C>{newValues};
	}
	ParallelArray<T, C> operator-(const ParallelArray<T, C>& other) const {
		T newValues[C];
		for(std::size_t i = 0; i < C; i++) {
			newValues[i] = values[i] - other.values[i];
		}
		return ParallelArray<T, C>{newValues};
	}
	template<typename T2>
	ParallelArray<T, C> operator*(T2& other) const {
		T newValues[C];
		for(std::size_t i = 0; i < C; i++) {
			newValues[i] = values[i] / other;
		}
		return ParallelArray<T, C>{newValues};
	}
	template<typename T2>
	ParallelArray<T, C> operator/(T2& other) const {
		T newValues[C];
		for(std::size_t i = 0; i < C; i++) {
			newValues[i] = values[i] / other;
		}
		return ParallelArray<T, C>{newValues};
	}

	void operator+=(const ParallelArray<T, C>& other) {
		for(std::size_t i = 0; i < C; i++) {
			values[i] += other.values[i];
		}
	}
	void operator-=(const ParallelArray<T, C>& other) {
		for(std::size_t i = 0; i < C; i++) {
			values[i] -= other.values[i];
		}
	}
	template<typename T2>
	void operator*=(T2& other) {
		for(std::size_t i = 0; i < C; i++) {
			values[i] *= other;
		}
	}
	template<typename T2>
	void operator/=(T2& other) {
		for(std::size_t i = 0; i < C; i++) {
			values[i] /= other;
		}
	}

	T& operator[](std::size_t index) {
		return values[index];
	}

	T sum() const {
		T total = values[0];
		for(std::size_t i = 1; i < C; i++) {
			total += values[i];
		}
		return total;
	}
};
};