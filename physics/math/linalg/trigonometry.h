#pragma once

#include "mat.h"

#include <cmath>


// vec
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
auto angleBetween(const Vector<T, Size>& first, const Vector<T, Size>& second) -> decltype(acos(normalize(first)* normalize(second))) {
	return acos(normalize(first) * normalize(second));
}

template<typename T, size_t Size>
Vector<T, Size> bisect(const Vector<T, Size>& first, const Vector<T, Size>& second) {
	return first * length(second) + second * length(first);
}

template<typename T, size_t Size>
size_t getMaxElementIndex(const Vector<T, Size>& vec) {
	size_t max = 0;

	for(size_t i = 1; i < Size; i++) {
		if(vec[i] > vec[max]) {
			max = i;
		}
	}
	return max;
}
template<typename T, size_t Size>
size_t getMinElementIndex(const Vector<T, Size>& vec) {
	size_t min = 0;

	for(size_t i = 1; i < Size; i++) {
		if(vec[i] > vec[min]) {
			min = i;
		}
	}
	return min;
}
template<typename T, size_t Size>
size_t getAbsMaxElementIndex(const Vector<T, Size>& vec) {
	size_t max = 0;

	for(size_t i = 1; i < Size; i++) {
		if(std::abs(vec[i]) > std::abs(vec[max])) {
			max = i;
		}
	}
	return max;
}

template<typename T, size_t Size>
size_t getAbsMinElementIndex(const Vector<T, Size>& vec) {
	size_t min = 0;

	for(size_t i = 1; i < Size; i++) {
		if(std::abs(vec[i]) < std::abs(vec[min])) {
			min = i;
		}
	}
	return min;
}

template<typename T, size_t Size>
T sumElements(const Vector<T, Size>& vec) {
	T sum = vec[0];
	for(size_t i = 1; i < Size; i++) {
		sum += vec[i];
	}
	return sum;
}

// mat3
template<typename T>
UnitaryMatrix<T, 3> rotMatX(T angle) {
	T sina = sin(angle);
	T cosa = cos(angle);
	return UnitaryMatrix<T, 3>{
		1, 0, 0,
		0, cosa, -sina,
		0, sina, cosa
	};
}
template<typename T>
UnitaryMatrix<T, 3> rotMatY(T angle) {
	T sina = sin(angle);
	T cosa = cos(angle);
	return UnitaryMatrix<T, 3>{
		cosa, 0, sina,
		0, 1, 0,
		-sina, 0, cosa
	};
}
template<typename T>
UnitaryMatrix<T, 3> rotMatZ(T angle) {
	T sina = sin(angle);
	T cosa = cos(angle);
	return UnitaryMatrix<T, 3>{
		cosa, -sina, 0,
		sina, cosa, 0,
		0, 0, 1
	};
}

template<typename T>
UnitaryMatrix<T, 3> faceMatX(Vector<T, 3> x, Vector<T, 3> yHint) {
	x = normalize(x);
	Vector<T, 3> z = normalize(x % yHint);
	Vector<T, 3> y = z % x;

	return UnitaryMatrix<T, 3>::fromColumns({x, y, z});
}
template<typename T>
UnitaryMatrix<T, 3> faceMatX(Vector<T, 3> x) {
	Vector<T, 3> yHint(0,0,0);
	yHint[getAbsMinElementIndex(x)] = 1;
	return faceMatX(x, yHint);
}
template<typename T>
UnitaryMatrix<T, 3> faceMatY(Vector<T, 3> y, Vector<T, 3> zHint) {
	y = normalize(y);
	Vector<T, 3> x = normalize(y % zHint);
	Vector<T, 3> z = x % y;

	return UnitaryMatrix<T, 3>::fromColumns({x, y, z});
}
template<typename T>
UnitaryMatrix<T, 3> faceMatY(Vector<T, 3> y) {
	Vector<T, 3> zHint(0, 0, 0);
	zHint[getAbsMinElementIndex(y)] = 1;
	return faceMatY(y, zHint);
}
template<typename T>
UnitaryMatrix<T, 3> faceMatZ(Vector<T, 3> z, Vector<T, 3> xHint) {
	z = normalize(z);
	Vector<T, 3> y = normalize(z % xHint);
	Vector<T, 3> x = y % z;

	return UnitaryMatrix<T, 3>::fromColumns({x, y, z});
}
template<typename T>
UnitaryMatrix<T, 3> faceMatZ(Vector<T, 3> z) {
	Vector<T, 3> xHint(0, 0, 0);
	xHint[getAbsMinElementIndex(z)] = 1;
	return faceMatZ(z, xHint);
}

/*
	Produces a rotation matrix from the provided euler angles

	Equivalent to rotMatZ(gamma) * rotMatX(alpha) * rotMatY(beta)
*/
template<typename T>
UnitaryMatrix<T, 3> rotationMatrixfromEulerAngles(T alpha, T beta, T gamma) {
	return rotMatZ(gamma) * rotMatX(alpha) * rotMatY(beta);
}

template<typename T>
Matrix<T, 3, 3> rotate(const Matrix<T, 3, 3> & mat, T angle, T x, T y, T z) {
	T s = sin(angle);
	T c = cos(angle);
	T C = 1 - c;

	Matrix<T, 3, 3> rotator{
		x * x * C + c,			x * y * C - z * s,		x * z * C + y * s,
		y * x * C + z * s,		y * y * C + c,			y * z * C - x * s,
		z * x * C - y * s,		z * y * C + x * s,		z * z * C + c
	};

	return mat * rotator;
}

template<typename T>
RotationMatrix<T, 3> rotationMatrixfromRotationVec(Vector<T, 3> rotVec) {

	T angle = length(rotVec);

	if (angle == 0) {
		return Matrix<T, 3, 3>::IDENTITY();
	}

	Vector<T, 3> normalizedRotVec = rotVec / angle;

	T x = normalizedRotVec.x;
	T y = normalizedRotVec.y;
	T z = normalizedRotVec.z;
	
	T sinA = sin(angle);
	T cosA = cos(angle);

	Matrix<T, 3, 3> outerProd = outer(normalizedRotVec, normalizedRotVec);
	Matrix<T, 3, 3> rotor{
		cosA,	    -z * sinA,   y * sinA,
		z * sinA,   cosA,		-x * sinA,
		-y * sinA,  x * sinA,	 cosA
	};

	return outerProd * (1 - cosA) + rotor;
}

template<typename T>
Vector<T, 3> rotationVectorfromRotationMatrix(const RotationMatrix<T, 3>& m) {
	Vector<T, 3> axisOfRotation(m[2][1] - m[1][2], m[0][2] - m[2][0], m[1][0] - m[0][1]);
	if(axisOfRotation[0] == 0 && axisOfRotation[1] == 0 && axisOfRotation[2] == 0) return Vector<T, 3>(0, 0, 0);
	double trace = m[0][0] + m[1][1] + m[2][2];

	double angle = acos((trace - 1) / 2);

	return normalize(axisOfRotation) * angle;
}

template<typename T>
Matrix<T, 3, 3> rotateAround(T angle, Vector<T, 3> normal) {
	// Using Rodrigues rotation formula;
	normal = normalize(normal);
	Matrix<T, 3, 3> W{
		 0,			-normal.z, normal.y,
		 normal.z,  0,		   -normal.x,
		 -normal.y, normal.x,  0
	};

	Matrix<T, 3, 3> W2 = W * W;
	T s = sin(angle);
	T s2 = sin(angle / 2);

	Matrix<T, 3, 3> R = Matrix<T, 3, 3>::IDENTITY() + W * s + W2 * (2 * s2 * s2);

	return R;
}


// mat4
Mat4f ortho(float left, float right, float bottom, float top, float zNear, float zFar);

Mat4f perspective(float fov, float aspect, float zNear, float zFar);

Mat4f lookAt(Vec3f from, Vec3f to, Vec3f up = Vec3f(0, 1, 0));

template<typename T>
Matrix<T, 4, 4> rotate(const Matrix<T, 4, 4>&, T angle, T x, T y, T z);

template<typename T>
Matrix<T, 4, 4> translate(const Matrix<T, 4, 4> & mat, T x, T y, T z) {
	Matrix<T, 1, 4> r{ x, y, z, 1.0 };
	Matrix<T, 1, 4> rr = mat * r;
	return joinHorizontal(mat.template getSubMatrix<3, 4>(0, 0), rr);
}

template<typename T>
Matrix<T, 4, 4> scale(const Matrix<T, 4, 4> & mat, const Vector<T, 3> & scaleVec) {
	Matrix<T, 4, 4> result;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			result[j][i] = mat[j][i] * scaleVec[i];
		}
	}
	for (int j = 0; j < 4; j++) {
		result[j][3] = mat[j][3];
	}

	return result;
}

template<typename T>
Matrix<T, 4, 4> scale(const Matrix<T, 4, 4> & mat, T x, T y, T z) {
	return scale(mat, Vector<T, 3>(x, y, z));
}

template<typename T>
Matrix<T, 4, 4> scale(const Matrix<T, 4, 4> & mat, T v) {
	return scale(mat, v, v, v);
}

template<typename T>
Matrix<T, 4, 4> translate(const Matrix<T, 4, 4> & mat, const Vector<T, 3> & dv) {
	return translate(mat, dv.x, dv.y, dv.z);
}
