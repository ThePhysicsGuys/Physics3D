#pragma once

#include <stddef.h>
#include <cmath>

template<typename T, size_t Size>
struct Vector {
	T data[Size];

	Vector() : data{} {
		for(size_t i = 0; i < Size; i++) {
			this->data[i] = 0;
		}
	}

	explicit Vector(T v) {
		for (size_t i = 0; i < Size; i++) {
			this->data[i] = v;
		}
	};

	Vector(const Vector<T, Size - 1>& other, T finalVal) {
		for (size_t i = 0; i < Size - 1; i++) {
			this->data[i] = other.data[i];
		}
		this->data[Size - 1] = finalVal;
	}

	template<typename OtherT>
	operator Vector<OtherT, Size>() const {
		Vector<OtherT, Size> result;
		for(size_t i = 0; i < Size; i++) {
			result.data[i] = static_cast<OtherT>(this->data[i]);
		}

		return result;
	}

	operator Vector<T, Size - 1>() const {
		Vector<T, Size + 1> result;
		for(size_t i = 0; i < Size - 1; i++) {
			result.data[i] = this->data[i];
		}

		return result;
	}

	T& operator[](size_t index) {
		return data[index];
	}

	const T& operator[](size_t index) const {
		return data[index];
	}
};

template<typename T>
struct Vector<T, 2> {
	union {
		struct { T x; T y; };
		T data[2];
	};

	Vector() : Vector(0, 0) {}
	Vector(T x, T y) : x(x), y(y) {}
	explicit Vector(T v) : x(v), y(v) {}

	template<typename OtherT>
	operator Vector<OtherT, 2>() const {
		return Vector<OtherT, 2>(static_cast<OtherT>(x), static_cast<OtherT>(y));
	}

	T& operator[](size_t index) {
		return data[index]; 
	}

	const T& operator[](size_t index) const { 
		return data[index]; 
	}
};

template<typename T>
struct Vector<T, 3> {
	union {
		struct { T x; T y; T z; };
		T data[3];
	};

	Vector() : Vector(0, 0, 0) {}
	Vector(T x, T y, T z) : x(x), y(y), z(z) {};
	explicit Vector(T v) : x(v), y(v), z(v) {}
	
	Vector(const Vector<T, 2>& other, T finalVal) : x(other.x), y(other.y), z(finalVal) {}

	template<typename OtherT>
	operator Vector<OtherT, 3>() const {
		return Vector<OtherT, 3>(static_cast<OtherT>(x), static_cast<OtherT>(y), static_cast<OtherT>(z));
	}

	operator Vector<T, 2>() const {
		return Vector<T, 2>(x, y);
	}

	T& operator[](size_t index) { 
		return data[index]; 
	}

	const T& operator[](size_t index) const { 
		return data[index];
	}
	
	static constexpr Vector<T, 3> X_AXIS() {
		return Vector<T, 3>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0));
	}

	static constexpr Vector<T, 3> Y_AXIS() {
		return Vector<T, 3>(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0));
	}

	static constexpr Vector<T, 3> Z_AXIS() {
		return Vector<T, 3>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));
	}

	static constexpr Vector<T, 3> X_AXIS_NEG() {
		return Vector<T, 3>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0));
	}

	static constexpr Vector<T, 3> Y_AXIS_NEG() {
		return Vector<T, 3>(static_cast<T>(0), static_cast<T>(-1), static_cast<T>(0));
	}

	static constexpr Vector<T, 3> Z_AXIS_NEG() {
		return Vector<T, 3>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1));
	}

	static constexpr Vector<T, 3> ZEROS() {
		return Vector<T, 3>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
	}
};

template<typename T>
struct Vector<T, 4> {
	union {
		struct { T x; T y; T z; T w; };
		T data[4];
	};
	Vector() : Vector(0,0,0,0) {}
	Vector(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
	explicit Vector(T v) : x(v), y(v), z(v), w(v) {}
	
	Vector(const Vector<T, 3>& other, T finalVal) : x(other.x), y(other.y), z(other.z), w(finalVal) {}

	template<typename OtherT>
	operator Vector<OtherT, 4>() const {
		return Vector<OtherT, 4>(static_cast<OtherT>(x), static_cast<OtherT>(y), static_cast<OtherT>(z), static_cast<OtherT>(w));
	}

	operator Vector<T, 3>() const {
		return Vector<T, 3>(x, y, z);
	}

	T& operator[](size_t index) { 
		return data[index]; 
	}

	const T& operator[](size_t index) const { 
		return data[index]; 
	}
}; 


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

typedef Vec2 NormalizedVec2;
typedef Vec3 NormalizedVec3;
typedef Vec4 NormalizedVec4;

template<typename T1, typename T2, size_t Size>
auto operator*(const Vector<T1, Size>& a, const Vector<T2, Size>& b) -> decltype(a[0]*b[0]+a[1]*b[1]) {
	decltype(a[0] * b[0] + a[1] * b[1]) result = a[0] * b[0];
	for (size_t i = 1; i < Size; i++) {
		result += a[i] * b[i];
	}
	return result;
}

template<typename T1, typename T2, size_t Size>
auto dot(const Vector<T1, Size>& a, const Vector<T2, Size>& b) -> decltype(a[0] * b[0] + a[1] * b[1]) {
	return a * b;
}

template<typename T, size_t Size>
auto dot(const Vector<T, Size>& vec) -> decltype(vec[0] * vec[0] + vec[1] * vec[1]) {
	return vec * vec;
}

template<typename T1, typename T2, size_t Size>
auto operator+(const Vector<T1, Size>& a, const Vector<T2, Size>& b) -> Vector<decltype(a[0] + b[0]), Size> {
	Vector<decltype(a[0] + b[0]), Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = a[i] + b[i];
	}
	return result;
}

template<typename T1, typename T2, size_t Size>
auto operator-(const Vector<T1, Size>& a, const Vector<T2, Size>& b) -> Vector<decltype(a[0] - b[0]), Size> {
	Vector<decltype(a[0] - b[0]), Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = a[i] - b[i];
	}
	return result;
}

template<typename T1, typename T2, size_t Size, typename = typename std::enable_if<std::is_arithmetic<T2>::value, T2>::type>
auto operator*(const Vector<T1, Size>& vec, const T2& factor) -> Vector<decltype(vec[0] * factor), Size> {
	Vector<decltype(vec[0] * factor), Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = vec[i] * factor;
	}
	return result;
}

template<typename T1, typename T2, size_t Size, typename = typename std::enable_if<std::is_arithmetic<T1>::value, T1>::type>
auto operator*(const T1& factor, const Vector<T2, Size>& vec) -> Vector<decltype(factor * vec[0]), Size> {
	Vector<decltype(factor * vec[0]), Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = factor * vec[i];
	}
	return result;
}

template<typename T1, typename T2, size_t Size, typename = typename std::enable_if<std::is_arithmetic<T2>::value, T2>::type>
auto operator/(const Vector<T1, Size>& vec, const T2& factor) -> Vector<decltype(vec[0] / factor), Size> {
	Vector<decltype(vec[0] / factor), Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = vec[i] / factor;
	}
	return result;
}

template<typename T, size_t Size>
Vector<T, Size> operator-(const Vector<T, Size>& vec) {
	Vector<T, Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = -vec[i];
	}
	return result;
}

template<typename T1, typename T2, size_t Size>
Vector<T1, Size>& operator+=(Vector<T1, Size>& vec, const Vector<T2, Size>& other) {
	for (size_t i = 0; i < Size; i++) {
		vec[i] += other[i];
	}
	return vec;
}

template<typename T1, typename T2, size_t Size>
Vector<T1, Size>& operator-=(Vector<T1, Size>& vec, const Vector<T2, Size>& other) {
	for (size_t i = 0; i < Size; i++) {
		vec[i] -= other[i];
	}
	return vec;
}

template<typename T1, typename T2, size_t Size>
Vector<T1, Size>& operator*=(Vector<T1, Size>& vec, const T2& factor) {
	for (size_t i = 0; i < Size; i++) {
		vec[i] *= factor;
	}
	return vec;
}

template<typename T1, typename T2, size_t Size>
Vector<T1, Size>& operator/=(Vector<T1, Size>& vec, const T2& factor) {
	for (size_t i = 0; i < Size; i++) {
		vec[i] /= factor;
	}
	return vec;
}

template<typename T1, typename T2>
auto operator%(const Vector<T1, 2>& first, const Vector<T2, 2>& second) -> decltype(first[0] * second[1] - first[1] * second[0]) {
	return first[0] * second[1] - first[1] * second[0];
}

template<typename T1, typename T2>
auto cross(const Vector<T1, 2>& first, const Vector<T2, 2>& second) -> decltype(first[0] * second[1] - first[1] * second[0]) {
	return first % second;
}

template<typename T1, typename T2>
auto operator%(const Vector<T1, 3>& first, const Vector<T2, 3>& second) -> Vector<decltype(first[1] * second[2] - first[2] * second[1]), 3> {
	return Vector<decltype(first[1] * second[2] - first[2] * second[1]), 3>{
		first[1] * second[2] - first[2] * second[1],
			first[2] * second[0] - first[0] * second[2],
			first[0] * second[1] - first[1] * second[0]
	};
}

template<typename T1, typename T2>
auto cross(const Vector<T1, 3>& first, const Vector<T2, 3>& second)->Vector<decltype(first[1] * second[2] - first[2] * second[1]), 3> {
	return first % second;
}

template<typename T, size_t Size>
bool operator==(const Vector<T, Size>& first, const Vector<T, Size>& second) {
	for(size_t i = 0; i < Size; i++)
		if(first[i] != second[i])
			return false;

	return true;
}

template<typename T, size_t Size>
bool operator!=(const Vector<T, Size>& first, const Vector<T, Size>& second) {
	return !(first == second);
}

template<typename T, size_t Size>
T lengthSquared(const Vector<T, Size>& vec) {
	T sum = vec[0] * vec[0];

	for (size_t i = 1; i < Size; i++) {
		sum += vec[i] * vec[i];
	}
	return sum;
}

template<typename T, size_t Size>
auto length(const Vector<T, Size>& vec) {
	return sqrt(lengthSquared(vec));
}

template<typename T>
T length(const Vector<T, 2>& vec) {
	return hypot(vec[0], vec[1]);
}

template<typename T1, typename T2, size_t Size>
bool isLongerThan(const Vector<T1, Size>& vec, const T2& length) {
	return lengthSquared(vec) > length * length;
}

template<typename T1, typename T2, size_t Size>
bool isShorterThan(const Vector<T1, Size>& vec, const T2& length) {
	return lengthSquared(vec) < length* length;
}

/**
* used to project the result of a dotproduct back onto the original vector
* @param v the result of dot(onto, otherVec)
* @return vec * (v/lengthSquared(vec))
*/
template<typename T, size_t Size>
Vector<T, Size> reProject(const Vector<T, Size>& onto, const T& v) {
	return onto * v / lengthSquared(onto);
}

/**
* projects vec onto onto
* @param vec vector to be projected
* @param onto vector to be projected on
* @return a projected version of the given vector
*/
template<typename T, size_t Size>
Vector<T, Size> project(const Vector<T, Size>& vec, const Vector<T, Size>& onto) {
	return onto * ((onto * vec) / lengthSquared(onto));
}


/**
* projects vec onto a plane with normal vector planeNormal
* @param vec vector to be projected
* @param planeNormal plane to be projected on
* @return a projected version of the given vector
*/
template<typename T, size_t Size>
Vector<T, Size> projectToPlaneNormal(const Vector<T, Size>& vec, const Vector<T, Size>& planeNormal) {
	return vec - vec * planeNormal * planeNormal / lengthSquared(planeNormal);
}

/**
* returns the distance of the given point to the line that goes through the origin along this vector
* @param point
* @return the distance
*/
template<typename T, size_t Size>
T pointToLineDistance(const Vector<T, Size>& line, const Vector<T, Size>& point) {
	return length(point - project(point, line));
}

/**
* returns the squared distance of the given point to the line that goes through the origin along this vector
* @param point
* @return the square of the distance
*/
template<typename T, size_t Size>
T pointToLineDistanceSquared(const Vector<T, Size>& line, const Vector<T, Size>& point) {
	return lengthSquared(point - project(point, line));
}

template<typename T, size_t Size>
Vector<T, Size> elementWiseMul(const Vector<T, Size>& first, const Vector<T, Size>& second) {
	Vector<T, Size> result;
	for (size_t i = 0; i < Size; i++)
		result[i] = first[i] * second[i];
	return result;
}

template<typename T, size_t Size>
Vector<T, Size> elementWiseSquare(const Vector<T, Size>& vec) {
	Vector<T, Size> result;
	for (size_t i = 0; i < Size; i++)
		result[i] = vec[i] * vec[i];
	return result;
}

template<typename T, size_t Size>
Vector<T, Size> elementWiseCube(const Vector<T, Size>& vec) {
	Vector<T, Size> result;
	for (size_t i = 0; i < Size; i++)
		result[i] = vec[i] * vec[i] * vec[i];
	return result;
}
