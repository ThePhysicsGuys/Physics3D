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
auto length(const Vector<T, Size>& vec) {
	return sqrt(lengthSquared(vec));
}

template<typename T>
T length(const Vector<T, 2> & vec) {
	return hypot(vec[0], vec[1]);
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



// mat2
template<typename T>
Matrix<T, 2, 2> fromAngle(T angle) {
	T sina = sin(angle);
	T cosa = cos(angle);
	return Matrix<T, 2, 2>{
		cosa, -sina,
		sina, cosa
	};
}


// mat3
template<typename N>
UnitaryMatrix<N, 3> rotX(N angle) {
	N sina = sin(angle);
	N cosa = cos(angle);
	return UnitaryMatrix<N, 3>{
		1, 0, 0,
		0, cosa, -sina,
		0, sina, cosa
	};
}
template<typename N>
UnitaryMatrix<N, 3> rotY(N angle) {
	N sina = sin(angle);
	N cosa = cos(angle);
	return UnitaryMatrix<N, 3>{
		cosa, 0, sina,
		0, 1, 0,
		-sina, 0, cosa
	};
}
template<typename N>
UnitaryMatrix<N, 3> rotZ(N angle) {
	N sina = sin(angle);
	N cosa = cos(angle);
	return UnitaryMatrix<N, 3>{
		cosa, -sina, 0,
		sina, cosa, 0,
		0, 0, 1
	};
}
/*
	Produces a rotation matrix from the provided euler angles

	Equivalent to rotZ(gamma) * rotX(alpha) * rotY(beta)
*/
template<typename N>
UnitaryMatrix<N, 3> fromEulerAngles(N alpha, N beta, N gamma) {
	return rotZ(gamma) * rotX(alpha) * rotY(beta);
}

template<typename N>
Matrix<N, 3, 3> rotate(const Matrix<N, 3, 3> & mat, N angle, N x, N y, N z) {
	N s = sin(angle);
	N c = cos(angle);
	N C = 1 - c;

	Matrix<N, 3, 3> rotator{
		x * x * C + c,			x * y * C - z * s,		x * z * C + y * s,
		y * x * C + z * s,		y * y * C + c,			y * z * C - x * s,
		z * x * C - y * s,		z * y * C + x * s,		z * z * C + c
	};

	return mat * rotator;
}

template<typename T>
RotationMatrix<T, 3> fromRotationVec(Vector<T, 3> rotVec) {

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
Vector<T, 3> fromRotationMatrix(const RotationMatrix<T, 3>& m) {
	Vector<T, 3> axisOfRotation(m[2][1] - m[1][2], m[0][2] - m[2][0], m[1][0] - m[0][1]);
	if(axisOfRotation[0] == 0 && axisOfRotation[0] == 0 && axisOfRotation[0] == 0) return Vector<T, 3>(0, 0, 0);
	double trace = m[0][0] + m[1][1] + m[2][2];

	double angle = acos((trace - 1) / 2);

	return normalize(axisOfRotation) * angle;
}

template<typename N>
Matrix<N, 3, 3> rotateAround(N angle, Vector<N, 3> normal) {
	// Using Rodrigues rotation formula;
	normal = normalize(normal);
	Matrix<N, 3, 3> W{
		 0,			-normal.z, normal.y,
		 normal.z,  0,		   -normal.x,
		 -normal.y, normal.x,  0
	};

	Matrix<N, 3, 3> W2 = W * W;
	N s = sin(angle);
	N s2 = sin(angle / 2);

	Matrix<N, 3, 3> R = Matrix<N, 3, 3>::IDENTITY() + W * s + W2 * (2 * s2 * s2);

	return R;
}


// mat4
Mat4f ortho(float left, float right, float bottom, float top, float zNear, float zFar);

Mat4f perspective(float fov, float aspect, float zNear, float zFar);

Mat4f lookAt(Vec3f from, Vec3f to, Vec3f up = Vec3f(0, 1, 0));

template<typename N>
Matrix<N, 4, 4> rotate(const Matrix<N, 4, 4>&, N angle, N x, N y, N z);

template<typename N>
Matrix<N, 4, 4> translate(const Matrix<N, 4, 4> & mat, N x, N y, N z) {
	Matrix<N, 1, 4> r{ x, y, z, 1.0 };
	Matrix<N, 1, 4> rr = mat * r;
	return joinHorizontal(mat.getSubMatrix<3, 4>(0, 0), rr);
}

template<typename N>
Matrix<N, 4, 4> scale(const Matrix<N, 4, 4> & mat, const Vector<N, 3> & scaleVec) {
	Matrix<N, 4, 4> result;

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

template<typename N>
Matrix<N, 4, 4> scale(const Matrix<N, 4, 4> & mat, N x, N y, N z) {
	return scale(mat, Vector<N, 3>(x, y, z));
}

template<typename N>
Matrix<N, 4, 4> scale(const Matrix<N, 4, 4> & mat, N v) {
	return scale(mat, v, v, v);
}

template<typename N>
Matrix<N, 4, 4> translate(const Matrix<N, 4, 4> & mat, const Vector<N, 3> & dv) {
	return translate(mat, dv.x, dv.y, dv.z);
}
