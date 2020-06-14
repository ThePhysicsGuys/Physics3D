#pragma once

#include "mat.h"

#include <cmath>

/*
	Creates a matrix such that for any vector x:
	v % x == createCrossProductEquivalent(v) * x
*/
template<typename T>
Matrix<T, 3, 3> createCrossProductEquivalent(const Vector<T, 3> & vec) {
	return Matrix<T, 3, 3>{
		0, -vec.z, vec.y,
		vec.z, 0, -vec.x,
		-vec.y, vec.x, 0
	};
}

/*
	Returns a matrix equivalent to (v % (v % x)) = skewSymmetricSquared(v) * x
	which is also equal to createCrossProductEquivalent(v)^2 = skewSymmetricSquared(v)
*/
template<typename N>
SymmetricMatrix<N, 3> skewSymmetricSquared(const Vector<N, 3> & v) {
	N x = v.x, y = v.y, z = v.z;
	return SymmetricMatrix<N, 3>{
		-(y * y + z * z),
		x* y, -(x * x + z * z),
		x* z, y* z, -(x * x + y * y)
	};
}

template<typename T>
SymmetricMatrix<T, 3> transformBasis(const SymmetricMatrix<T, 3> & sm, const Matrix<T, 3, 3> & rotation) {
	Matrix<T, 3, 3> r = rotation * sm * ~rotation;
	return SymmetricMatrix<T, 3>{
		r[0][0],
		r[1][0], r[1][1],
		r[2][0], r[2][1], r[2][2]
	};
}

template<typename T>
SymmetricMatrix<T, 3> transformBasis(const DiagonalMatrix<T, 3> & dm, const Matrix<T, 3, 3> & rotation) {
	Matrix<T, 3, 3> r = rotation * dm * ~rotation;
	return SymmetricMatrix<T, 3>{
		r[0][0],
		r[1][0], r[1][1],
		r[2][0], r[2][1], r[2][2]
	};
}

template<typename T>
SymmetricMatrix<T, 3> multiplyLeftRight(const SymmetricMatrix<T, 3> & sm, const Matrix<T, 3, 3> & otherMat) {
	Matrix<T, 3, 3> r = otherMat * sm * otherMat.transpose();
	return SymmetricMatrix<T, 3>{
		r[0][0],
		r[1][0], r[1][1],
		r[2][0], r[2][1], r[2][2]
	};
}

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
	T angleSq = lengthSquared(rotVec);
	T angle = sqrt(angleSq);

	// sinc(angle) = sin(angle) / angle
	// around angle=0 =~ 1 - angle^2 / 6 + angle^4 / 120 - angle^6 / 5040 
	T sincAngle = (angleSq > 1E-20) ? sin(angle) / angle : 1.0 - angleSq / 6.0;

	T cosAngle = cos(angle);

	// cosc(angle) = (cos(angle) - 1) / angle^2
	// around angle=0 =~ 1/2 - angle^2 / 24 + angle^4 / 720 - angle^6 / 40320
	T coscAngle = (angleSq > 1E-20) ? (1 - cosAngle) / angleSq : 0.5 - angleSq / 24.0;

	Vector<T, 3> sincVec = rotVec * sincAngle;
	Matrix<T, 3, 3> rotor{
		 cosAngle,   -sincVec.z,   sincVec.y,
		 sincVec.z,   cosAngle,   -sincVec.x,
		-sincVec.y,   sincVec.x,   cosAngle
	};

	return outer(rotVec, rotVec) * coscAngle + rotor;
}

template<typename T>
Vector<T, 3> rotationVectorfromRotationMatrix(const RotationMatrix<T, 3>& m) {
	Vector<T, 3> axisOfRotation(m[2][1] - m[1][2], m[0][2] - m[2][0], m[1][0] - m[0][1]);
	if(axisOfRotation[0] == 0 && axisOfRotation[1] == 0 && axisOfRotation[2] == 0) return Vector<T, 3>(0, 0, 0);
	T trace = m[0][0] + m[1][1] + m[2][2];

	T angle = acos((trace - 1) / 2);

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

Mat4f lookAt(const Vec3f& from, const Vec3f& to, const Vec3f& up = Vec3f(0, 1.0f, 0));

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
