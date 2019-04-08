#pragma once

template<typename T, size_t C>
struct ParallelArray {
	T values[C];

	ParallelArray() = default;

	ParallelArray(T values[C]) {
		for(size_t i = 0; i < C; i++) {
			this->values[i] = values[i];
		}
	}

	ParallelArray(const ParallelArray& other) {
		for(size_t i = 0; i < C; i++) {
			this->values[i] = other.values[i];
		}
	}

	void operator=(const ParallelArray& other) {
		for(size_t i = 0; i < C; i++) {
			this->values[i] = other.values[i];
		}
	}

	ParallelArray<T, C> operator+(const ParallelArray<T, C>& other) const {
		T newValues[C];
		for(int i = 0; i < C; i++) {
			newValues[i] = values[i] + other.values[i];
		}
		return ParallelArray<T, C>{newValues};
	}
	ParallelArray<T, C> operator-(const ParallelArray<T, C>& other) const {
		T newValues[C];
		for(int i = 0; i < C; i++) {
			newValues[i] = values[i] - other.values[i];
		}
		return ParallelArray<T, C>{newValues};
	}
	template<typename T2>
	ParallelArray<T, C> operator*(T2& other) const {
		T newValues[C];
		for(int i = 0; i < C; i++) {
			newValues[i] = values[i] / other;
		}
		return ParallelArray<T, C>{newValues};
	}
	template<typename T2>
	ParallelArray<T, C> operator/(T2& other) const {
		T newValues[C];
		for(int i = 0; i < C; i++) {
			newValues[i] = values[i] / other;
		}
		return ParallelArray<T, C>{newValues};
	}

	void operator+=(const ParallelArray<T, C>& other) {
		for(int i = 0; i < C; i++) {
			values[i] += other.values[i];
		}
	}
	void operator-=(const ParallelArray<T, C>& other) {
		for(int i = 0; i < C; i++) {
			values[i] -= other.values[i];
		}
	}
	template<typename T2>
	void operator*=(T2& other) {
		for(int i = 0; i < C; i++) {
			values[i] *= other;
		}
	}
	template<typename T2>
	void operator/=(T2& other) {
		for(int i = 0; i < C; i++) {
			values[i] /= other;
		}
	}

	T& operator[](size_t index) {
		return values[index];
	}

	T sum() const {
		T total = values[0];
		for(size_t i = 1; i < C; i++) {
			total += values[i];
		}
		return total;
	}
};
