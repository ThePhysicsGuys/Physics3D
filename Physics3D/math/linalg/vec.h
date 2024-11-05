#pragma once

#include <stddef.h>
#include <cmath>
#include <assert.h>
#include <experimental/simd>
#include <iostream>
namespace P3D {
template<typename T, size_t Size>
struct Vector {
	T data[Size];

	constexpr Vector() noexcept : data{} {
		for(T& item : this->data) {
			item = 0;
		}
	}

	template<typename OtherT>
	constexpr operator Vector<OtherT, Size>() const noexcept {
		Vector<OtherT, Size> result;
		for(size_t i = 0; i < Size; i++) {
			result.data[i] = static_cast<OtherT>(this->data[i]);
		}

		return result;
	}

	constexpr size_t size() const { return Size; }

	constexpr T& operator[](size_t index) noexcept {
		return data[index];
	}

	constexpr const T& operator[](size_t index) const noexcept {
		return data[index];
	}

	static constexpr Vector<T, Size> full(T v) noexcept {
		Vector<T, Size> result;
		for(size_t i = 0; i < Size; i++) {
			result[i] = v;
		}
		return result;
	}

	template<size_t SubSize>
	constexpr Vector<T, SubSize> getSubVector(size_t startingAt = 0) const noexcept {
		Vector<T, SubSize> result;
		for(size_t i = 0; i < SubSize; i++) {
			result[i] = this->data[i + startingAt];
		}
		return result;
	}
};

template<typename T>
struct Vector<T, 1> {
	union {
		T data[1];
		struct { T x; };
	};

	constexpr Vector() noexcept : data{0} {}
	constexpr Vector(T x) noexcept : data{x} {}

	template<typename OtherT>
	constexpr operator Vector<OtherT, 1>() const noexcept {
		return Vector<OtherT, 1>(static_cast<OtherT>(this->data[0]));
	}

	constexpr size_t size() const { return 1; }

	constexpr T& operator[](size_t index) noexcept {
		return data[index];
	}

	constexpr const T& operator[](size_t index) const noexcept {
		return data[index];
	}

	static constexpr Vector<T, 1> full(T v) noexcept {
		return Vector<T, 1>(v);
	}

	template<size_t SubSize>
	constexpr Vector<T, SubSize> getSubVector(size_t startingAt = 0) const noexcept {
		Vector<T, SubSize> result;
		for(size_t i = 0; i < SubSize; i++) {
			result[i] = this->data[i + startingAt];
		}
		return result;
	}

	constexpr operator T() const { return data[0]; }
};

template<typename T>
struct Vector<T, 2> {
	union {
		T data[2];
		struct { T x; T y; };
	};

	constexpr Vector() noexcept : data{0, 0} {}
	constexpr Vector(T x, T y) noexcept : data{x, y} {}

	template<typename OtherT>
	constexpr operator Vector<OtherT, 2>() const noexcept {
		return Vector<OtherT, 2>(static_cast<OtherT>(this->data[0]), static_cast<OtherT>(this->data[1]));
	}

	constexpr size_t size() const { return 2; }

	constexpr T& operator[](size_t index) noexcept {
		return data[index];
	}

	constexpr const T& operator[](size_t index) const noexcept {
		return data[index];
	}

	static constexpr Vector<T, 2> full(T v) noexcept {
		return Vector<T, 2>(v, v);
	}

	template<size_t SubSize>
	constexpr Vector<T, SubSize> getSubVector(size_t startingAt = 0) const noexcept {
		Vector<T, SubSize> result;
		for(size_t i = 0; i < SubSize; i++) {
			result[i] = this->data[i + startingAt];
		}
		return result;
	}
};

template<typename T>
struct Vector<T, 3> {
	union {
		T data[3];
		struct { T x; T y; T z; };
	};

	constexpr Vector() noexcept : data{0, 0, 0} {}
	constexpr Vector(T x, T y, T z) noexcept : data{x, y, z} {};

	template<typename OtherT>
	constexpr operator Vector<OtherT, 3>() const noexcept {
		return Vector<OtherT, 3>(static_cast<OtherT>(this->data[0]), static_cast<OtherT>(this->data[1]), static_cast<OtherT>(this->data[2]));
	}

	constexpr size_t size() const { return 3; }

	constexpr T& operator[](size_t index) noexcept {
		return data[index];
	}

	constexpr const T& operator[](size_t index) const noexcept {
		return data[index];
	}

	static constexpr Vector<T, 3> full(T v) noexcept {
		return Vector<T, 3>(v, v, v);
	}

	template<size_t SubSize>
	constexpr Vector<T, SubSize> getSubVector(size_t startingAt = 0) const noexcept {
		Vector<T, SubSize> result;
		for(size_t i = 0; i < SubSize; i++) {
			result[i] = this->data[i + startingAt];
		}
		return result;
	}
};

template<typename T>
struct Vector<T, 4> {
	union {
		T data[4];
		struct { T x; T y; T z; T w; };
	};
	constexpr Vector() noexcept : data{0, 0, 0, 0} {}
	constexpr Vector(T x, T y, T z, T w) noexcept : data{x, y, z, w} {}

	template<typename OtherT>
	constexpr operator Vector<OtherT, 4>() const noexcept {
		return Vector<OtherT, 4>(static_cast<OtherT>(this->data[0]), static_cast<OtherT>(this->data[1]), static_cast<OtherT>(this->data[2]), static_cast<OtherT>(this->data[3]));
	}

	constexpr size_t size() const { return 4; }

	constexpr T& operator[](size_t index) noexcept {
		return data[index];
	}

	constexpr const T& operator[](size_t index) const noexcept {
		return data[index];
	}

	static constexpr Vector<T, 4> full(T v) noexcept {
		return Vector<T, 4>(v, v, v, v);
	}

	template<size_t SubSize>
	constexpr Vector<T, SubSize> getSubVector(size_t startingAt = 0) const noexcept {
		Vector<T, SubSize> result;
		for(size_t i = 0; i < SubSize; i++) {
			result[i] = this->data[i + startingAt];
		}
		return result;
	}
};

typedef Vector<double, 1>		Vec1;
typedef Vector<float, 1>		Vec1f;
typedef Vector<long long, 1>	Vec1l;
typedef Vector<int, 1>			Vec1i;

typedef Vector<double, 2>		Vec2;
typedef Vector<float, 2>		Vec2f;
typedef Vector<long long, 2>	Vec2l;
typedef Vector<int, 2>			Vec2i;

typedef Vector<double, 3>		Vec3;
typedef Vector<float, 3>		Vec3f;
typedef Vector<long long, 3>	Vec3l;
typedef Vector<int, 3>			Vec3i;

typedef Vector<double, 4>		Vec4;
typedef Vector<float, 4>		Vec4f;
typedef Vector<long long, 4>	Vec4l;
typedef Vector<int, 4>			Vec4i;

typedef Vector<double, 5>		Vec5;
typedef Vector<float, 5>		Vec5f;
typedef Vector<long long, 5>	Vec5l;
typedef Vector<int, 5>			Vec5i;

typedef Vector<double, 6>		Vec6;
typedef Vector<float, 6>		Vec6f;
typedef Vector<long long, 6>	Vec6l;
typedef Vector<int, 6>			Vec6i;


// template<typename T1, typename T2, size_t Size>
// constexpr auto operator*(const Vector<T1, Size>& a, const Vector<T2, Size>& b) noexcept -> decltype(a[0] * b[0] + a[1] * b[1]) {
// 	decltype(a[0] * b[0] + a[1] * b[1]) result = a[0] * b[0];
// 	for(size_t i = 1; i < Size; i++) {
// 		result += a[i] * b[i];
// 	}
// 	return result;
// }

namespace stdx = std::experimental;


template<typename T1, typename T2, size_t Size>
requires std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>
constexpr auto operator*(const Vector<T1, Size>& a, const Vector<T2, Size>& b) noexcept -> decltype(a[0] * b[0]) {
	stdx::fixed_size_simd<decltype(a[0] * b[0]), Size> result;
	stdx::fixed_size_simd<T1, Size> a_simd(&a[0], stdx::element_aligned);
	stdx::fixed_size_simd<T2, Size> b_simd(&b[0], stdx::element_aligned);
	result = a_simd * b_simd;
	return stdx::reduce(result);
	// decltype(a[0] * b[0] + a[1] * b[1]) result = a[0] * b[0];
	// for(size_t i = 1; i < Size; i++) {
	// 	result += a[i] * b[i];
	// }
	// return result;
}
template<typename T1, typename T2, size_t Size>
requires (!(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>))
constexpr auto operator*(const Vector<T1, Size>& a, const Vector<T2, Size>& b) noexcept -> decltype(a[0] * b[0]) {
	decltype(a[0] * b[0]) result = a[0] * b[0];
	for(size_t i = 1; i < Size; i++) {
		result += a[i] * b[i];
	}
	return result;
}

template<typename T1, typename T2, size_t Size>
constexpr auto dot(const Vector<T1, Size>& a, const Vector<T2, Size>& b) noexcept -> decltype(a[0] * b[0]) {
	return a * b;
}

template<typename T, size_t Size>
constexpr auto dot(const Vector<T, Size>& vec) noexcept -> decltype(vec[0] * vec[0]) {
	return vec * vec;
}

// template<typename T1, typename T2, size_t Size>
// constexpr auto operator+(const Vector<T1, Size>& a, const Vector<T2, Size>& b) noexcept -> Vector<decltype(a[0] + b[0]), Size> {
// 	Vector<decltype(a[0] + b[0]), Size> result;
// 	for(size_t i = 0; i < Size; i++) {
// 		result[i] = a[i] + b[i];
// 	}
// 	return result;
// }

template<typename T1, typename T2, size_t Size>
requires std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>
constexpr auto operator+(const Vector<T1, Size>& a, const Vector<T2, Size>& b) noexcept -> Vector<decltype(a[0] + b[0]), Size> {
	stdx::fixed_size_simd<decltype(a[0] + b[0]), Size> result;
	stdx::fixed_size_simd<T1, Size> a_simd(&a[0], stdx::element_aligned);
	stdx::fixed_size_simd<T2, Size> b_simd(&b[0], stdx::element_aligned);
	result = a_simd + b_simd;
	Vector<decltype(a[0] + b[0]), Size> result_vec;
	result.copy_to(result_vec.data, stdx::element_aligned);
	return result_vec;
	// Vector<decltype(a[0] + b[0]), Size> result;
	// for(size_t i = 0; i < Size; i++) {
	// 	result[i] = a[i] + b[i];
	// }
	// return result;
}

template<typename T1, typename T2, size_t Size>
requires (!(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>))
constexpr auto operator+(const Vector<T1, Size>& a, const Vector<T2, Size>& b) noexcept -> Vector<decltype(a[0] + b[0]), Size> {
	Vector<decltype(a[0] + b[0]), Size> result;
	for(size_t i = 0; i < Size; i++) {
		result[i] = a[i] + b[i];
	}
	return result;
}

template<typename T1, typename T2, size_t Size>
requires (!(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>))
constexpr auto operator-(const Vector<T1, Size>& a, const Vector<T2, Size>& b) noexcept -> Vector<decltype(a[0] - b[0]), Size> {
	Vector<decltype(a[0] - b[0]), Size> result;
	for(size_t i = 0; i < Size; i++) {
		result[i] = a[i] - b[i];
	}
	return result;
}

template<typename T1, typename T2, size_t Size>
requires std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>
constexpr auto operator-(const Vector<T1, Size>& a, const Vector<T2, Size>& b) noexcept -> Vector<decltype(a[0] - b[0]), Size> {
	stdx::fixed_size_simd<decltype(a[0] - b[0]), Size> result;
	stdx::fixed_size_simd<T1, Size> a_simd(&a[0], stdx::element_aligned);
	stdx::fixed_size_simd<T2, Size> b_simd(&b[0], stdx::element_aligned);
	result = a_simd - b_simd;
	Vector<decltype(a[0] - b[0]), Size> result_vec;
	result.copy_to(result_vec.data, stdx::element_aligned);
	return result_vec;

	// Vector<decltype(a[0] - b[0]), Size> result;
	// for(size_t i = 0; i < Size; i++) {
	// 	result[i] = a[i] - b[i];
	// }
	// return result;
}

template<typename T1, typename T2, size_t Size>
requires (!(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>))
constexpr auto operator*(const Vector<T1, Size>& vec, const T2& factor) noexcept -> Vector<decltype(vec[0] * factor), Size> {
	Vector<decltype(vec[0] * factor), Size> result;
	for(size_t i = 0; i < Size; i++) {
		result[i] = vec[i] * factor;
	}
	return result;
}

template<typename T1, typename T2, size_t Size>
requires std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>
constexpr auto operator*(const Vector<T1, Size>& vec, const T2& factor) noexcept -> Vector<decltype(vec[0] * factor), Size> {
	// store decltype(vec[0] * factor) as a data type for use later
	decltype(vec[0] * factor) type;
	stdx::fixed_size_simd<decltype(type), Size> result;
	stdx::fixed_size_simd<decltype(type), Size> vec_simd(&vec[0], stdx::element_aligned);
	stdx::fixed_size_simd<decltype(type), Size> factor_simd = factor;
	result = vec_simd * factor_simd;
	Vector<decltype(type), Size> result_vec;
	result.copy_to(result_vec.data, stdx::element_aligned);
	return result_vec;
	// Vector<decltype(vec[0] * factor), Size> result;
	// for(size_t i = 0; i < Size; i++) {
	// 	result[i] = vec[i] * factor;
	// }
	// return result;
}

template<typename T1, typename T2, size_t Size>
requires (!(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>))
constexpr auto operator*(const T1& factor, const Vector<T2, Size>& vec) noexcept -> Vector<decltype(factor* vec[0]), Size> {
	Vector<decltype(factor* vec[0]), Size> result;
	for(size_t i = 0; i < Size; i++) {
		result[i] = factor * vec[i];
	}
	return result;
}

template<typename T1, typename T2, size_t Size>
requires std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>
constexpr auto operator*(const T1& factor, const Vector<T2, Size>& vec) noexcept -> Vector<decltype(factor* vec[0]), Size> {
	decltype(factor* vec[0]) type;
	stdx::fixed_size_simd<decltype(type), Size> result;
	stdx::fixed_size_simd<decltype(type), Size> factor_simd = factor;
	stdx::fixed_size_simd<decltype(type), Size> vec_simd(&vec[0], stdx::element_aligned);
	result = factor_simd * vec_simd;
	Vector<decltype(type), Size> result_vec;
	result.copy_to(result_vec.data, stdx::element_aligned);
	return result_vec;

	// Vector<decltype(factor* vec[0]), Size> result;
	// for(size_t i = 0; i < Size; i++) {
	// 	result[i] = factor * vec[i];
	// }
	// return result;
}

template<typename T1, typename T2, size_t Size>
requires (!(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>))
constexpr auto operator/(const Vector<T1, Size>& vec, const T2& factor) noexcept -> Vector<decltype(vec[0] / factor), Size> {
	Vector<decltype(vec[0] / factor), Size> result;
	for(size_t i = 0; i < Size; i++) {
		result[i] = vec[i] / factor;
	}
	return result;
}

template<typename T1, typename T2, size_t Size>
requires std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>
constexpr auto operator/(const Vector<T1, Size>& vec, const T2& factor) noexcept -> Vector<decltype(vec[0] / factor), Size> {
	decltype(vec[0] / factor) type;
	stdx::fixed_size_simd<decltype(type), Size> result;
	stdx::fixed_size_simd<decltype(type), Size> vec_simd(&vec[0], stdx::element_aligned);
	stdx::fixed_size_simd<decltype(type), Size> factor_simd = factor;
	result = vec_simd / factor_simd;
	Vector<decltype(type), Size> result_vec;
	result.copy_to(result_vec.data, stdx::element_aligned);
	return result_vec;

	// Vector<decltype(vec[0] / factor), Size> result;
	// for(size_t i = 0; i < Size; i++) {
	// 	result[i] = vec[i] / factor;
	// }
	// return result;
}

template<typename T, size_t Size>
requires (!(std::is_arithmetic_v<T>))
constexpr Vector<T, Size> operator-(const Vector<T, Size>& vec) noexcept {
	Vector<T, Size> result;
	for(size_t i = 0; i < Size; i++) {
		result[i] = -vec[i];
	}
	return result;
}

template<typename T, size_t Size>
requires std::is_arithmetic_v<T>
constexpr Vector<T, Size> operator-(const Vector<T, Size>& vec) noexcept {
	stdx::fixed_size_simd<T, Size> result(&vec[0], stdx::element_aligned);
	result = -result;
	Vector<T, Size> result_vec;
	result.copy_to(result_vec.data, stdx::element_aligned);
	return result_vec;

	// Vector<T, Size> result;
	// for(size_t i = 0; i < Size; i++) {
	// 	result[i] = -vec[i];
	// }
	// return result;
}

template<typename T1, typename T2, size_t Size>
requires (!(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>))
constexpr Vector<T1, Size>& operator+=(Vector<T1, Size>& vec, const Vector<T2, Size>& other) noexcept {
	for(size_t i = 0; i < Size; i++) {
		vec[i] += other[i];
	}
	return vec;
}

template<typename T1, typename T2, size_t Size>
requires std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>
constexpr Vector<T1, Size>& operator+=(Vector<T1, Size>& vec, const Vector<T2, Size>& other) noexcept {
	stdx::fixed_size_simd<T1, Size> vec_simd(&vec[0], stdx::element_aligned);
	stdx::fixed_size_simd<T2, Size> other_simd(&other[0], stdx::element_aligned);
	vec_simd += other_simd;
	vec_simd.copy_to(vec.data, stdx::element_aligned);
	return vec;

	// for(size_t i = 0; i < Size; i++) {
	// 	vec[i] += other[i];
	// }
	// return vec;
}

template<typename T1, typename T2, size_t Size>
requires (!(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>))
constexpr Vector<T1, Size>& operator-=(Vector<T1, Size>& vec, const Vector<T2, Size>& other) noexcept {
	for(size_t i = 0; i < Size; i++) {
		vec[i] -= other[i];
	}
	return vec;
}

template<typename T1, typename T2, size_t Size>
requires std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>
constexpr Vector<T1, Size>& operator-=(Vector<T1, Size>& vec, const Vector<T2, Size>& other) noexcept {
	stdx::fixed_size_simd<T1, Size> vec_simd(&vec[0], stdx::element_aligned);
	stdx::fixed_size_simd<T2, Size> other_simd(&other[0], stdx::element_aligned);
	vec_simd -= other_simd;
	vec_simd.copy_to(vec.data, stdx::element_aligned);
	return vec;

	// for(size_t i = 0; i < Size; i++) {
	// 	vec[i] -= other[i];
	// }
	// return vec;
}

template<typename T1, typename T2, size_t Size>
requires (!(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>))
constexpr Vector<T1, Size>& operator*=(Vector<T1, Size>& vec, const T2& factor) noexcept {
	for(size_t i = 0; i < Size; i++) {
		vec[i] *= factor;
	}
	return vec;
}

template<typename T1, typename T2, size_t Size>
requires std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>
constexpr Vector<T1, Size>& operator*=(Vector<T1, Size>& vec, const T2& factor) noexcept {
	stdx::fixed_size_simd<T1, Size> vec_simd(&vec[0], stdx::element_aligned);
	stdx::fixed_size_simd<T2, Size> factor_simd = factor;
	vec_simd *= factor_simd;
	vec_simd.copy_to(vec.data, stdx::element_aligned);
	return vec;

	// for(size_t i = 0; i < Size; i++) {
	// 	vec[i] *= factor;
	// }
	// return vec;
}

template<typename T1, typename T2, size_t Size>
requires (!(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>))
constexpr Vector<T1, Size>& operator/=(Vector<T1, Size>& vec, const T2& factor) noexcept {
	for(size_t i = 0; i < Size; i++) {
		vec[i] /= factor;
	}
	return vec;
}

template<typename T1, typename T2, size_t Size>
requires std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>
constexpr Vector<T1, Size>& operator/=(Vector<T1, Size>& vec, const T2& factor) noexcept {
	stdx::fixed_size_simd<T1, Size> vec_simd(&vec[0], stdx::element_aligned);
	stdx::fixed_size_simd<T2, Size> factor_simd = factor;
	vec_simd /= factor_simd;
	vec_simd.copy_to(vec.data, stdx::element_aligned);
	return vec;

	// for(size_t i = 0; i < Size; i++) {
	// 	vec[i] /= factor;
	// }
	// return vec;
}

template<typename T1, typename T2>
constexpr auto operator%(const Vector<T1, 2>& first, const Vector<T2, 2>& second) noexcept -> decltype(first[0] * second[1] - first[1] * second[0]) {
	return first[0] * second[1] - first[1] * second[0];
}

template<typename T1, typename T2>
constexpr auto cross(const Vector<T1, 2>& first, const Vector<T2, 2>& second) noexcept -> decltype(first[0] * second[1] - first[1] * second[0]) {
	return first % second;
}

template<typename T1, typename T2>
constexpr auto operator%(const Vector<T1, 3>& first, const Vector<T2, 3>& second) noexcept -> Vector<decltype(first[1] * second[2] - first[2] * second[1]), 3> {
	return Vector<decltype(first[1] * second[2] - first[2] * second[1]), 3>{
		first[1] * second[2] - first[2] * second[1],
			first[2] * second[0] - first[0] * second[2],
			first[0] * second[1] - first[1] * second[0]
	};
}

template<typename T1, typename T2>
constexpr auto cross(const Vector<T1, 3>& first, const Vector<T2, 3>& second) noexcept -> Vector<decltype(first[1] * second[2] - first[2] * second[1]), 3> {
	return first % second;
}

template<typename T, size_t Size>
constexpr bool operator==(const Vector<T, Size>& first, const Vector<T, Size>& second) noexcept {
	for(size_t i = 0; i < Size; i++)
		if(first[i] != second[i])
			return false;

	return true;
}

template<typename T, size_t Size>
constexpr bool operator!=(const Vector<T, Size>& first, const Vector<T, Size>& second) noexcept {
	return !(first == second);
}


template<typename T, size_t Size1, size_t Size2>
constexpr Vector<T, Size1 + Size2> join(const Vector<T, Size1>& first, const Vector<T, Size2>& second) noexcept {
	Vector<T, Size1 + Size2> result;
	for(size_t i = 0; i < Size1; i++) {
		result[i] = first[i];
	}
	for(size_t i = 0; i < Size2; i++) {
		result[i + Size1] = second[i];
	}
	return result;
}
template<typename T, size_t Size>
constexpr Vector<T, Size + 1> join(const Vector<T, Size>& vec, const T& extraValue) noexcept {
	Vector<T, Size + 1> result;
	for(size_t i = 0; i < Size; i++) {
		result[i] = vec[i];
	}
	result[Size] = extraValue;
	return result;
}
template<typename T, size_t Size>
constexpr Vector<T, Size + 1> join(const T& extraValue, const Vector<T, Size>& vec) noexcept {
	Vector<T, Size + 1> result;
	result[0] = extraValue;
	for(size_t i = 0; i < Size; i++) {
		result[i + 1] = vec[i];
	}
	return result;
}
template<typename T, size_t Size>
constexpr Vector<T, Size - 1> withoutIndex(const Vector<T, Size>& vec, size_t indexToRemove) {
	assert(indexToRemove < Size);
	Vector<T, Size - 1> result;
	for(size_t i = 0; i < indexToRemove; i++) {
		result[i] = vec[i];
	}
	for(size_t i = indexToRemove + 1; i < Size; i++) {
		result[i - 1] = vec[i];
	}
	return result;
}



template<typename T, size_t Size>
constexpr T lengthSquared(const Vector<T, Size>& vec) noexcept {
	return vec * vec;
}

template<typename T, size_t Size>
constexpr T length(const Vector<T, Size>& vec) noexcept {
	return std::sqrt(lengthSquared(vec));
}

template<typename T>
constexpr T length(const Vector<T, 2>& vec) noexcept {
	return std::hypot(vec[0], vec[1]);
}

template<typename T1, typename T2, size_t Size>
constexpr bool isLongerThan(const Vector<T1, Size>& vec, const T2& length) noexcept {
	return lengthSquared(vec) > length * length;
}

template<typename T1, typename T2, size_t Size>
constexpr bool isShorterThan(const Vector<T1, Size>& vec, const T2& length) noexcept {
	return lengthSquared(vec) < length * length;
}

// vec
template<typename T, size_t Size>
constexpr Vector<T, Size> withLength(const Vector<T, Size>& vec, const T& newLength) noexcept {
	return vec * (newLength / length(vec));
}

template<typename T, size_t Size>
constexpr Vector<T, Size> maxLength(const Vector<T, Size>& vec, const T& maxLength) noexcept {
	if(isLongerThan(vec, maxLength))
		return withLength(vec, maxLength);
	else
		return vec;
}

template<typename T, size_t Size>
constexpr Vector<T, Size> minLength(const Vector<T, Size>& vec, const T& minLength) noexcept {
	if(isShorterThan(vec, minLength))
		return withLength(vec, minLength);
	else
		return vec;
}

template<typename T, size_t Size>
constexpr Vector<T, Size> normalize(const Vector<T, Size>& vec) noexcept {
	return vec / length(vec);
}

// template<typename T, size_t Size>
// constexpr Vector<T, Size> abs(const Vector<T, Size>& vec) noexcept {
// 	Vector<T, Size> result;
// 	for(size_t i = 0; i < Size; i++)
// 		result[i] = std::abs(vec[i]);
// 	return result;
// }

template<typename T, size_t Size>
constexpr Vector<T, Size> abs(const Vector<T, Size>& vec) noexcept {
	stdx::fixed_size_simd<T, Size> vec_simd(&vec[0], stdx::element_aligned);
	vec_simd = stdx::abs(vec_simd);
	Vector<T, Size> result_vec;
	vec_simd.copy_to(result_vec.data, stdx::element_aligned);
	return result_vec;

	// Vector<T, Size> result;
	// for(size_t i = 0; i < Size; i++)
	// 	result[i] = std::abs(vec[i]);
	// return result;
}

/**
* used to project the result of a dotproduct back onto the original vector
* @param v the result of dot(onto, otherVec)
* @return vec * (v/lengthSquared(vec))
*/
template<typename T, size_t Size>
constexpr Vector<T, Size> reProject(const Vector<T, Size>& onto, const T& v) noexcept {
	return onto * v / lengthSquared(onto);
}

/**
* projects vec onto onto
* @param vec vector to be projected
* @param onto vector to be projected on
* @return a projected version of the given vector
*/
template<typename T, size_t Size>
constexpr Vector<T, Size> project(const Vector<T, Size>& vec, const Vector<T, Size>& onto) noexcept {
	return onto * ((onto * vec) / lengthSquared(onto));
}


/**
* projects vec onto a plane with normal vector planeNormal
* @param vec vector to be projected
* @param planeNormal plane to be projected on
* @return a projected version of the given vector
*/
template<typename T, size_t Size>
constexpr Vector<T, Size> projectToPlaneNormal(const Vector<T, Size>& vec, const Vector<T, Size>& planeNormal) noexcept {
	return vec - vec * planeNormal * planeNormal / lengthSquared(planeNormal);
}

/**
* returns the distance of the given point to the line that goes through the origin along this vector
* @param point
* @return the distance
*/
template<typename T, size_t Size>
constexpr T pointToLineDistance(const Vector<T, Size>& line, const Vector<T, Size>& point) noexcept {
	return length(point - project(point, line));
}

/**
* returns the squared distance of the given point to the line that goes through the origin along this vector
* @param point
* @return the square of the distance
*/
template<typename T, size_t Size>
constexpr T pointToLineDistanceSquared(const Vector<T, Size>& line, const Vector<T, Size>& point) noexcept {
	return lengthSquared(point - project(point, line));
}


template<typename T, size_t Size>
requires (!(std::is_arithmetic_v<T>))
constexpr Vector<T, Size> elementWiseMul(const Vector<T, Size>& first, const Vector<T, Size>& second) noexcept {
	Vector<T, Size> result;
	for(size_t i = 0; i < Size; i++)
		result[i] = first[i] * second[i];
	return result;
}
template<typename T, size_t Size>
requires std::is_arithmetic_v<T>
constexpr Vector<T, Size> elementWiseMul(const Vector<T, Size>& first, const Vector<T, Size>& second) noexcept {
	stdx::fixed_size_simd<T, Size> result;
	stdx::fixed_size_simd<T, Size> first_simd(&first[0], stdx::element_aligned);
	stdx::fixed_size_simd<T, Size> second_simd(&second[0], stdx::element_aligned);
	result = first_simd * second_simd;
	Vector<T, Size> result_vec;
	result.copy_to(result_vec.data, stdx::element_aligned);
	return result_vec;

	// Vector<T, Size> result;
	// for(size_t i = 0; i < Size; i++)
	// 	result[i] = first[i] * second[i];
	// return result;
}

template<typename T, size_t Size>
requires (!(std::is_arithmetic_v<T>))
constexpr Vector<T, Size> elementWiseSquare(const Vector<T, Size>& vec) noexcept {
	Vector<T, Size> result;
	for(size_t i = 0; i < Size; i++)
		result[i] = vec[i] * vec[i];
	return result;
}

template<typename T, size_t Size>
requires std::is_arithmetic_v<T>
constexpr Vector<T, Size> elementWiseSquare(const Vector<T, Size>& vec) noexcept {
	stdx::fixed_size_simd<T, Size> result;
	stdx::fixed_size_simd<T, Size> vec_simd(&vec[0], stdx::element_aligned);
	result = vec_simd * vec_simd;
	Vector<T, Size> result_vec;
	result.copy_to(result_vec.data, stdx::element_aligned);
	return result_vec;

	// Vector<T, Size> result;
	// for(size_t i = 0; i < Size; i++)
	// 	result[i] = vec[i] * vec[i];
	// return result;
}

template<typename T, size_t Size>
requires (!(std::is_arithmetic_v<T>))
constexpr Vector<T, Size> elementWiseCube(const Vector<T, Size>& vec) noexcept {
	Vector<T, Size> result;
	for(size_t i = 0; i < Size; i++)
		result[i] = vec[i] * vec[i] * vec[i];
	return result;
}

template<typename T, size_t Size>
requires std::is_arithmetic_v<T>
constexpr Vector<T, Size> elementWiseCube(const Vector<T, Size>& vec) noexcept {
	stdx::fixed_size_simd<T, Size> result;
	stdx::fixed_size_simd<T, Size> vec_simd(&vec[0], stdx::element_aligned);
	result = vec_simd * vec_simd * vec_simd;
	Vector<T, Size> result_vec;
	result.copy_to(result_vec.data, stdx::element_aligned);
	return result_vec;

	// Vector<T, Size> result;
	// for(size_t i = 0; i < Size; i++)
	// 	result[i] = vec[i] * vec[i] * vec[i];
	// return result;
}

/* computes (
	a.y * b.z + a.z * b.y,
	a.z * b.x + a.x * b.z,
	a.x * b.y + a.y * b.x
)*/
template<typename T>
constexpr Vector<T, 3> mulOppositesBiDir(const Vector<T, 3>& a, const Vector<T, 3>& b) noexcept {
	return Vector<T, 3>(
		a[1] * b[2] + a[2] * b[1],
		a[2] * b[0] + a[0] * b[2],
		a[0] * b[1] + a[1] * b[0]
		);
}
// computes (vec.y * vec.z, vec.z * vec.x, vec.x * vec.y)
template<typename T>
constexpr Vector<T, 3> mulSelfOpposites(const Vector<T, 3>& vec) noexcept {
	return Vector<T, 3>(vec[1] * vec[2], vec[2] * vec[0], vec[0] * vec[1]);
}
// computes (vec.y + vec.z, vec.z + vec.x, vec.x + vec.y)
template<typename T>
constexpr Vector<T, 3> addSelfOpposites(const Vector<T, 3>& vec) noexcept {
	return Vector<T, 3>(vec[1] + vec[2], vec[2] + vec[0], vec[0] + vec[1]);
}

template<typename T, size_t Size>
constexpr size_t getMaxElementIndex(const Vector<T, Size>& vec) noexcept {
	size_t max = 0;

	for(size_t i = 1; i < Size; i++) {
		if(vec[i] > vec[max]) {
			max = i;
		}
	}
	return max;
}
template<typename T, size_t Size>
constexpr size_t getMinElementIndex(const Vector<T, Size>& vec) noexcept {
	size_t min = 0;

	for(size_t i = 1; i < Size; i++) {
		if(vec[i] > vec[min]) {
			min = i;
		}
	}
	return min;
}
template<typename T, size_t Size>
constexpr size_t getAbsMaxElementIndex(const Vector<T, Size>& vec) noexcept {
	size_t max = 0;

	for(size_t i = 1; i < Size; i++) {
		if(std::abs(vec[i]) > std::abs(vec[max])) {
			max = i;
		}
	}
	return max;
}

template<typename T, size_t Size>
constexpr size_t getAbsMinElementIndex(const Vector<T, Size>& vec) noexcept {
	size_t min = 0;

	for(size_t i = 1; i < Size; i++) {
		if(std::abs(vec[i]) < std::abs(vec[min])) {
			min = i;
		}
	}
	return min;
}

template<typename T, size_t Size>
constexpr T sumElements(const Vector<T, Size>& vec) noexcept {
	T sum = vec[0];
	for(size_t i = 1; i < Size; i++) {
		sum += vec[i];
	}
	return sum;
}

template<typename T, size_t Size>
constexpr auto angleBetween(const Vector<T, Size>& first, const Vector<T, Size>& second) noexcept -> decltype(acos(normalize(first)* normalize(second))) {
	return acos(normalize(first) * normalize(second));
}

template<typename T, size_t Size>
constexpr Vector<T, Size> bisect(const Vector<T, Size>& first, const Vector<T, Size>& second) noexcept {
	return first * length(second) + second * length(first);
}
};
