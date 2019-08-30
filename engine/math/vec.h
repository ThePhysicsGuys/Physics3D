#pragma once

#include <cmath>


template<typename T, size_t Size>
struct Vector {
	T data[Size];

	Vector() {
		for (size_t i = 0; i < Size; i++) {
			this->data[i] = 0;
		}
	}
	template<typename OtherT>
	Vector(const Vector<OtherT, Size>& other) {
		for (size_t i = 0; i < Size; i++) {
			this->data[i] = static_cast<T>(other.data[i]);
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
	Vector(const Vector<T, Size + 1> & other) {
		for (size_t i = 0; i < Size; i++) {
			this->data[i] = other.data[i];
		}
	}

	T& operator[](size_t index) { return data[index]; }
	const T& operator[](size_t index) const { return data[index]; }
};

template<typename T>
struct Vector<T, 2> {
	union {
		struct { T x; T y; };
		T data[2];
	};
	Vector() : x(0), y(0) {}
	Vector(T x, T y) : x(x), y(y) {}
	explicit Vector(T v) : x(v), y(v) {}
	template<typename OtherT>
	Vector(const Vector<OtherT, 2>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}
	explicit Vector(const Vector<T, 3> & other) : x(other.x), y(other.y) {}
	
	T& operator[](size_t index) { return data[index]; }
	const T& operator[](size_t index) const { return data[index]; }
};

template<typename T>
struct Vector<T, 3> {
	union {
		struct { T x; T y; T z; };
		T data[3];
	};
	Vector() : x(0), y(0), z(0) {}
	Vector(T x, T y, T z) : x(x), y(y), z(z) {};
	explicit Vector(T v) : x(v), y(v), z(v) {}
	template<typename OtherT>
	Vector(const Vector<OtherT, 3>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)) {}
	Vector(const Vector<T, 2>& other, T z) : x(other.x), y(other.y), z(z) {}
	explicit Vector(const Vector<T, 4> & other) : x(other.x), y(other.y), z(other.z) {}

	T& operator[](size_t index) { return data[index]; }
	const T& operator[](size_t index) const { return data[index]; }

	Vector<T, 3> rotateAround(const Vector<T, 3> other, T angle) const {
		T s = sin(angle);
		T c = cos(angle);

		return *this * c + (other % *this) * s + other * (other * *this) * (1 - c);
	}
};

template<typename T>
struct Vector<T, 4> {
	union {
		struct { T x; T y; T z; T w; };
		T data[4];
	};
	Vector() : x(0), y(0), z(0), w(0) {}
	Vector(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {};
	explicit Vector(T v) : x(v), y(v), z(v), w(v) {}
	template<typename OtherT>
	Vector(const Vector<OtherT, 4>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)), w(static_cast<T>(other.w)) {}
	Vector(const Vector<T, 3>& other, T w) : x(other.x), y(other.y), z(other.z), w(w) {}
	explicit Vector(const Vector<T, 5> & other) : x(other[0]), y(other[1]), z(other[2]), w(other[3]) {}

	T& operator[](size_t index) { return data[index]; }
	const T& operator[](size_t index) const { return data[index]; }
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




template<typename T, typename T2, size_t Size>
auto operator*(const Vector<T, Size>& a, const Vector<T2, Size>& b) -> decltype(a[0]*b[0]+a[1]*b[1]) {
	decltype(a[0] * b[0] + a[1] * b[1]) result = a[0] * b[0];
	for (size_t i = 1; i < Size; i++) {
		result += a[i] * b[i];
	}
	return result;
}

template<typename T, typename T2, size_t Size>
auto operator+(const Vector<T, Size>& a, const Vector<T2, Size>& b) -> Vector<decltype(a[0] + b[0]), Size> {
	Vector<decltype(a[0] + b[0]), Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = a[i] + b[i];
	}
	return result;
}

template<typename T, typename T2, size_t Size>
auto operator-(const Vector<T, Size>& a, const Vector<T2, Size>& b) -> Vector<decltype(a[0] - b[0]), Size> {
	Vector<decltype(a[0] - b[0]), Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = a[i] - b[i];
	}
	return result;
}

template<typename T, typename T2, size_t Size>
auto operator*(const Vector<T, Size>& vec, const T2& factor) -> Vector<decltype(vec[0] * factor), Size> {
	Vector<decltype(vec[0] * factor), Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = vec[i] * factor;
	}
	return result;
}

template<typename T, size_t Size>
auto operator*(const T& factor, const Vector<T, Size>& vec) -> Vector<decltype(factor * vec[0]), Size> {
	Vector<decltype(factor * vec[0]), Size> result;
	for (size_t i = 0; i < Size; i++) {
		result[i] = factor * vec[i];
	}
	return result;
}

template<typename T, typename T2, size_t Size>
auto operator/(const Vector<T, Size>& vec, const T2& factor) -> Vector<decltype(vec[0] / factor), Size> {
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

template<typename T, typename T2, size_t Size>
Vector<T, Size>& operator+=(Vector<T, Size>& vec, const Vector<T2, Size>& other) {
	for (size_t i = 0; i < Size; i++) {
		vec[i] += other[i];
	}
	return vec;
}
template<typename T, typename T2, size_t Size>
Vector<T, Size>& operator-=(Vector<T, Size>& vec, const Vector<T2, Size>& other) {
	for (size_t i = 0; i < Size; i++) {
		vec[i] -= other[i];
	}
	return vec;
}
template<typename T, typename T2, size_t Size>
Vector<T, Size>& operator*=(Vector<T, Size>& vec, const T2& factor) {
	for (size_t i = 0; i < Size; i++) {
		vec[i] *= factor;
	}
	return vec;
}
template<typename T, typename T2, size_t Size>
Vector<T, Size>& operator/=(Vector<T, Size>& vec, const T2& factor) {
	for (size_t i = 0; i < Size; i++) {
		vec[i] /= factor;
	}
	return vec;
}

template<typename T, typename T2>
auto operator%(const Vector<T, 2>& first, const Vector<T2, 2>& second) -> decltype(first[0] * second[1] - first[1] * second[0]) {
	return first[0] * second[1] - first[1] * second[0];
}

template<typename T, typename T2>
auto operator%(const Vector<T, 3>& first, const Vector<T2, 3>& second) -> Vector<decltype(first[1] * second[2] - first[2] * second[1]), 3> {
	return Vector<decltype(first[1] * second[2] - first[2] * second[1]), 3>{
		first[1] * second[2] - first[2] * second[1],
			first[2] * second[0] - first[0] * second[2],
			first[0] * second[1] - first[1] * second[0]
	};
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

/*template<typename T>
T length(const Vector<T, 3>& vec) {
	return hypot(vec[0], vec[1], vec[2]);
}*/

template<typename T, size_t Size>
Vector<T, Size> normalize(const Vector<T, Size>& vec) {
	return vec / length(vec);
}

template<typename T, size_t Size>
Vector<T, Size> abs(const Vector<T, Size>& vec) {
	Vector<T, Size> result;
	for (size_t i = 0; i < Size; i++)
		result[i] = fabs(vec[i]);
	return result;
}

template<typename T, size_t Size>
bool isLongerThan(const Vector<T, Size>& vec, const T& length) {
	return lengthSquared(vec) > length * length;
}

template<typename T, size_t Size>
bool isShorterThan(const Vector<T, Size>& vec, const T& length) {
	return lengthSquared(vec) < length* length;
}

template<typename T, size_t Size>
Vector<T, Size> withLength(const Vector<T, Size>& vec, const T& newLength) {
	return vec * (newLength / length(vec));
}

template<typename T, size_t Size>
Vector<T, Size> maxLength(const Vector<T, Size>& vec, const T& maxLength) {
	if (isLongerThan(vec, maxLength))
		return withLength(vec, maxLength);
	else
		return vec;
}

template<typename T, size_t Size>
Vector<T, Size> minLength(const Vector<T, Size>& vec, const T& minLength) {
	if (isShorterThan(vec, minLength))
		return withLength(vec, minLength);
	else
		return vec;
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

template<typename T, size_t Size>
auto angleBetween(const Vector<T, Size>& first, const Vector<T, Size>& second) -> decltype(acos(normalize(first)* normalize(second))) {
	return acos(normalize(first) * normalize(second));
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
Vector<T, Size> bisect(const Vector<T, Size>& first, const Vector<T, Size>& second) {
	return first * length(second) + second * length(first);
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
