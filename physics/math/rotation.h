#pragma once

#include "linalg/mat.h"
#include "linalg/trigonometry.h"
#include <assert.h>
#include <cmath>


template<typename T>
class RotationTemplate {
	Matrix<T, 3, 3> rotationMatrix;

	constexpr explicit RotationTemplate(const Matrix<T, 3, 3>& rotationMatrix);
public:

	constexpr RotationTemplate();

	Vector<T, 3> localToGlobal(const Vector<T, 3>& vec) const;
	Vector<T, 3> globalToLocal(const Vector<T, 3>& vec) const;
	RotationTemplate localToGlobal(const RotationTemplate<T>& rot) const;
	RotationTemplate globalToLocal(const RotationTemplate<T>& rot) const;

	/*
		Transforms the given symmetric matrix into the basis of this rotation

		effectively brings a local matrix into the global space

		rot.localToGlobal(sm * v) == rot.localToGlobal(sm) * rot.localToGlobal(v)
		== rot * sm * ~rot * rot * v
	*/
	SymmetricMatrix<T, 3> localToGlobal(const SymmetricMatrix<T, 3>& sm) const;

	/*
		Transforms the given symmetric matrix from the basis of this rotation

		effectively brings a global matrix into the local space

		rot.globalToLocal(sm * v) == rot.globalToLocal(sm) * rot.globalToLocal(v)
		== ~rot * sm * rot * ~rot * v
	*/
	SymmetricMatrix<T, 3> globalToLocal(const SymmetricMatrix<T, 3>& sm) const;

	RotationTemplate operator~() const;
	RotationTemplate operator*(const RotationTemplate& rot2) const;
	RotationTemplate& operator*=(const RotationTemplate& rot2);
	Vec3 operator*(const Vector<T, 3>& vec) const;

	static RotationTemplate rotX(double angle);
	static RotationTemplate rotY(double angle);
	static RotationTemplate rotZ(double angle);
	static RotationTemplate fromEulerAngles(double alpha, double beta, double gamma);
	static RotationTemplate fromRotationVec(const Vector<T, 3>& rotationVector);
	static RotationTemplate fromRotationMatrix(const Matrix<T, 3, 3>& rotationMatrix);

	static RotationTemplate faceX(Vector<T, 3> xDirection);
	static RotationTemplate faceX(Vector<T, 3> xDirection, Vector<T, 3> yHint);
	static RotationTemplate faceY(Vector<T, 3> yDirection);
	static RotationTemplate faceY(Vector<T, 3> yDirection, Vector<T, 3> zHint);
	static RotationTemplate faceZ(Vector<T, 3> zDirection);
	static RotationTemplate faceZ(Vector<T, 3> zDirection, Vector<T, 3> xHint);

	Matrix<T, 3, 3> asRotationMatrix() const;
	Vector<T, 3> asRotationVector() const;

	operator Matrix<T, 3, 3>() const {
		return this->asRotationMatrix();
	}

	template<typename OtherT>
	operator RotationTemplate<OtherT>() const;

	struct Predefined;
};

typedef RotationTemplate<double> Rotation;
typedef RotationTemplate<float> Rotationf;

template<typename T>
struct RotationTemplate<T>::Predefined {
	static constexpr RotationTemplate<T> IDENTITY{Matrix<T, 3, 3>{1,0,0,0,1,0,0,0,1}};

	static constexpr RotationTemplate<T> X_90 {Matrix<T, 3, 3>{ 1, 0, 0, 0, 0,-1, 0, 1, 0}};
	static constexpr RotationTemplate<T> Y_90 {Matrix<T, 3, 3>{ 0, 0, 1, 0, 1, 0,-1, 0, 0}};
	static constexpr RotationTemplate<T> Z_90 {Matrix<T, 3, 3>{ 0,-1, 0, 1, 0, 0, 0, 0, 1}};

	static constexpr RotationTemplate<T> X_180{Matrix<T, 3, 3>{ 1, 0, 0, 0,-1, 0, 0, 0,-1}};
	static constexpr RotationTemplate<T> Y_180{Matrix<T, 3, 3>{-1, 0, 0, 0, 1, 0, 0, 0,-1}};
	static constexpr RotationTemplate<T> Z_180{Matrix<T, 3, 3>{-1, 0, 0, 0,-1, 0, 0, 0, 1}};

	static constexpr RotationTemplate<T> X_270{Matrix<T, 3, 3>{ 1, 0, 0, 0, 0, 1, 0,-1, 0}};
	static constexpr RotationTemplate<T> Y_270{Matrix<T, 3, 3>{ 0, 0,-1, 0, 1, 0, 1, 0, 0}};
	static constexpr RotationTemplate<T> Z_270{Matrix<T, 3, 3>{ 0, 1, 0,-1, 0, 0, 0, 0, 1}};
};


template<typename T>
inline constexpr RotationTemplate<T>::RotationTemplate(const Matrix<T, 3, 3>& rotationMatrix) : rotationMatrix(rotationMatrix) {}

template<typename T>
inline constexpr RotationTemplate<T>::RotationTemplate() : rotationMatrix(Matrix<T, 3, 3>::IDENTITY()) {}

template<typename T>
inline Vector<T, 3> RotationTemplate<T>::localToGlobal(const Vector<T, 3>& vec) const {
	return rotationMatrix * vec;
}

template<typename T>
inline Vector<T, 3> RotationTemplate<T>::globalToLocal(const Vector<T, 3>& vec) const {
	return rotationMatrix.transpose() * vec;
}

template<typename T>
inline RotationTemplate<T> RotationTemplate<T>::localToGlobal(const RotationTemplate<T>& rot) const {
	return (*this) * rot;
}

template<typename T>
inline RotationTemplate<T> RotationTemplate<T>::globalToLocal(const RotationTemplate<T>& rot) const {
	return (~*this) * rot;
}

/*
Transforms the given symmetric matrix into the basis of this rotation

effectively brings a local matrix into the global space

rot.localToGlobal(sm * v) == rot.localToGlobal(sm) * rot.localToGlobal(v)
== rot * sm * ~rot * rot * v
*/
template<typename T>
inline SymmetricMatrix<T, 3> RotationTemplate<T>::localToGlobal(const SymmetricMatrix<T, 3>& sm) const {
	Matrix<T, 3, 3> r = this->rotationMatrix * sm * this->rotationMatrix.transpose();
	return SymmetricMatrix<T, 3>{
		r[0][0],
			r[1][0], r[1][1],
			r[2][0], r[2][1], r[2][2]
	};
}

/*
Transforms the given symmetric matrix from the basis of this rotation

effectively brings a global matrix into the local space

rot.globalToLocal(sm * v) == rot.globalToLocal(sm) * rot.globalToLocal(v)
== ~rot * sm * rot * ~rot * v
*/
template<typename T>
inline SymmetricMatrix<T, 3> RotationTemplate<T>::globalToLocal(const SymmetricMatrix<T, 3>& sm) const {
	Matrix<T, 3, 3> r = this->rotationMatrix.transpose() * sm * this->rotationMatrix;
	return SymmetricMatrix<T, 3>{
		r[0][0],
			r[1][0], r[1][1],
			r[2][0], r[2][1], r[2][2]
	};
}

template<typename T>
RotationTemplate<T> RotationTemplate<T>::operator~() const {
	return RotationTemplate(rotationMatrix.transpose());
}

template<typename T>
RotationTemplate<T> RotationTemplate<T>::operator*(const RotationTemplate<T>& rot2) const {
	return RotationTemplate(this->rotationMatrix * rot2.rotationMatrix);
}

template<typename T>
RotationTemplate<T>& RotationTemplate<T>::operator*=(const RotationTemplate<T>& rot2) {
	this->rotationMatrix = this->rotationMatrix * rot2.rotationMatrix;
	return *this;
}

template<typename T>
Vec3 RotationTemplate<T>::operator*(const Vector<T, 3>& vec) const {
	return rotationMatrix * vec;
}

template<typename T>
RotationTemplate<T> RotationTemplate<T>::rotX(double angle) {
	return RotationTemplate<T>(::rotMatX(angle));
}
template<typename T>
RotationTemplate<T> RotationTemplate<T>::rotY(double angle) {
	return RotationTemplate<T>(::rotMatY(angle));
}
template<typename T>
RotationTemplate<T> RotationTemplate<T>::rotZ(double angle) {
	return RotationTemplate<T>(::rotMatZ(angle));
}

template<typename T>
RotationTemplate<T> RotationTemplate<T>::fromEulerAngles(double alpha, double beta, double gamma) {
	return RotationTemplate<T>::rotZ(gamma) * RotationTemplate<T>::rotX(alpha) * RotationTemplate<T>::rotY(beta);
}

template<typename T>
RotationTemplate<T> RotationTemplate<T>::fromRotationMatrix(const Matrix<T, 3, 3>& rotationMatrix) {
	assert(isValidRotationMatrix(rotationMatrix));
	return RotationTemplate<T>(rotationMatrix);
}

template<typename T>
RotationTemplate<T> RotationTemplate<T>::fromRotationVec(const Vector<T, 3>& rotationVector) {
	return RotationTemplate<T>::fromRotationMatrix(::rotationMatrixfromRotationVec(rotationVector));
}

template<typename T>
Matrix<T, 3, 3> RotationTemplate<T>::asRotationMatrix() const {
	return this->rotationMatrix;
}
template<typename T>
Vector<T, 3> RotationTemplate<T>::asRotationVector() const {
	return ::rotationVectorfromRotationMatrix(this->rotationMatrix);
}

template<typename T>
template<typename OtherT>
RotationTemplate<T>::operator RotationTemplate<OtherT>() const {
	return RotationTemplate<OtherT>::fromRotationMatrix(static_cast<Matrix<OtherT, 3, 3>>(this->rotationMatrix));
}

template<typename T>
RotationTemplate<T> RotationTemplate<T>::faceX(Vector<T, 3> xDirection) {
	return RotationTemplate<T>::fromRotationMatrix(::faceMatX(xDirection));
}
template<typename T>
RotationTemplate<T> RotationTemplate<T>::faceX(Vector<T, 3> xDirection, Vector<T, 3> yHint) {
	return RotationTemplate<T>::fromRotationMatrix(::faceMatX(xDirection, yHint));
}
template<typename T>
RotationTemplate<T> RotationTemplate<T>::faceY(Vector<T, 3> yDirection) {
	return RotationTemplate<T>::fromRotationMatrix(::faceMatY(yDirection));
}
template<typename T>
RotationTemplate<T> RotationTemplate<T>::faceY(Vector<T, 3> yDirection, Vector<T, 3> zHint) {
	return RotationTemplate<T>::fromRotationMatrix(::faceMatY(yDirection, zHint));
}
template<typename T>
RotationTemplate<T> RotationTemplate<T>::faceZ(Vector<T, 3> zDirection) {
	return RotationTemplate<T>::fromRotationMatrix(::faceMatZ(zDirection));
}
template<typename T>
RotationTemplate<T> RotationTemplate<T>::faceZ(Vector<T, 3> zDirection, Vector<T, 3> xHint) {
	return RotationTemplate<T>::fromRotationMatrix(::faceMatZ(zDirection, xHint));
}


template<typename T>
bool isValidRotationMatrix(const Matrix<T, 3, 3>& mat) {
	struct Comparator {
		static bool equalsApproximately(double value, double comparedTo) {
			auto delta = comparedTo - value;
			return delta < 1E-8 && delta > -1E-8;
		}
		static bool equalsApproximately(float value, float comparedTo) {
			auto delta = comparedTo - value;
			return delta < 1E-4 && delta > -1E-4;
		}
	};
	constexpr T zero = 0;
	constexpr T one = 1;
	for(int i = 0; i < 3; i++) {
		T rowLengthSq = lengthSquared(mat.getRow(i));
		T colLengthSq = lengthSquared(mat.getCol(i));
		if(!Comparator::equalsApproximately(rowLengthSq, one)
		|| !Comparator::equalsApproximately(colLengthSq, one)) {
			return false;
		}
	}

	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			if(i == j) continue;

			if(!Comparator::equalsApproximately(mat.getRow(i) * mat.getRow(j), zero)
			|| !Comparator::equalsApproximately(mat.getCol(i) * mat.getCol(j), zero)) {
				return false;
			}
		}
	}

	T detMat = det(mat);
	if(!Comparator::equalsApproximately(detMat, one)) {
		return false;
	}
	return true;
}

template<typename T>
Matrix<T, 3, 3> operator*(const Matrix<T, 3, 3>& matrix, const RotationTemplate<T>& rotation) {
	return matrix * rotation.asRotationMatrix();
}

template<typename T>
Matrix<T, 3, 3> operator*(const RotationTemplate<T>& rotation, const Matrix<T, 3, 3>& matrix) {
	return rotation.asRotationMatrix() * matrix;
}
