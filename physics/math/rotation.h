#pragma once

#include "linalg/mat.h"
#include "linalg/trigonometry.h"
#include "linalg/quat.h"
#include "constants.h"

#include <assert.h>
#include <cmath>


template<typename T>
class MatrixRotationTemplate {
	Matrix<T, 3, 3> rotationMatrix;
	constexpr explicit MatrixRotationTemplate(const Matrix<T, 3, 3>& rotationMatrix) : rotationMatrix(rotationMatrix) {};
public:

	constexpr MatrixRotationTemplate();

	Vector<T, 3> localToGlobal(const Vector<T, 3>& vec) const;
	Vector<T, 3> globalToLocal(const Vector<T, 3>& vec) const;
	MatrixRotationTemplate localToGlobal(const MatrixRotationTemplate<T>& rot) const;
	MatrixRotationTemplate globalToLocal(const MatrixRotationTemplate<T>& rot) const;

	/*
		Transforms the given symmetric matrix into the basis of this rotation

		effectively brings a local matrix into the global space

		defined as   rot * sm * ~rot

		rot.localToGlobal(sm * v) == rot.localToGlobal(sm) * rot.localToGlobal(v)
		== (rot * sm * ~rot) * rot * v == (rot * sm) * v == rot * (sm * v)
	*/
	SymmetricMatrix<T, 3> localToGlobal(const SymmetricMatrix<T, 3>& sm) const;

	/*
		Transforms the given symmetric matrix from the basis of this rotation

		effectively brings a global matrix into the local space

		defined as   ~rot * sm * rot

		rot.globalToLocal(sm * v) == rot.globalToLocal(sm) * rot.globalToLocal(v)
		== (~rot * sm * rot) * ~rot * v == (~rot * sm) * v == ~rot * (sm * v)
	*/
	SymmetricMatrix<T, 3> globalToLocal(const SymmetricMatrix<T, 3>& sm) const;

	MatrixRotationTemplate operator~() const;
	MatrixRotationTemplate operator*(const MatrixRotationTemplate& rot2) const;
	MatrixRotationTemplate& operator*=(const MatrixRotationTemplate& rot2);
	Vector<T, 3> operator*(const Vector<T, 3>& vec) const;

	/* returns the projection of the x-axis
		== this->localToGlobal(Vec3(1,0,0)) */
	Vector<T, 3> getX() const;
	/* returns the projection of the y-axis
		== this->localToGlobal(Vec3(0,1,0)) */
	Vector<T, 3> getY() const;
	/* returns the projection of the z-axis
		== this->localToGlobal(Vec3(0,0,1)) */
	Vector<T, 3> getZ() const;

	static MatrixRotationTemplate rotX(T angle);
	static MatrixRotationTemplate rotY(T angle);
	static MatrixRotationTemplate rotZ(T angle);
	static MatrixRotationTemplate fromEulerAngles(T alpha, T beta, T gamma) {
		return MatrixRotationTemplate<T>::rotZ(gamma) * MatrixRotationTemplate<T>::rotX(alpha) * MatrixRotationTemplate<T>::rotY(beta);
	}
	static MatrixRotationTemplate fromRotationVec(const Vector<T, 3>& rotationVector);
	static MatrixRotationTemplate fromRotationMatrix(const Matrix<T, 3, 3>& rotationMatrix);
	static MatrixRotationTemplate fromRotationQuaternion(const Quaternion<T>& rotationQuaternion);


	static MatrixRotationTemplate faceX(Vector<T, 3> xDirection);
	static MatrixRotationTemplate faceX(Vector<T, 3> xDirection, Vector<T, 3> yHint);
	static MatrixRotationTemplate faceY(Vector<T, 3> yDirection);
	static MatrixRotationTemplate faceY(Vector<T, 3> yDirection, Vector<T, 3> zHint);
	static MatrixRotationTemplate faceZ(Vector<T, 3> zDirection);
	static MatrixRotationTemplate faceZ(Vector<T, 3> zDirection, Vector<T, 3> xHint);

	Matrix<T, 3, 3> asRotationMatrix() const;
	Quaternion<T> asRotationQuaternion() const;
	Vector<T, 3> asRotationVector() const;

	/*operator Matrix<T, 3, 3>() const {
		return this->asRotationMatrix();
	}*/

	template<typename OtherT>
	operator MatrixRotationTemplate<OtherT>() const;

	struct Predefined{
		static constexpr MatrixRotationTemplate<T> IDENTITY{Matrix<T, 3, 3>{1,0,0,0,1,0,0,0,1}};

		static constexpr MatrixRotationTemplate<T> X_90 {Matrix<T, 3, 3>{ 1, 0, 0, 0, 0,-1, 0, 1, 0}};
		static constexpr MatrixRotationTemplate<T> Y_90 {Matrix<T, 3, 3>{ 0, 0, 1, 0, 1, 0,-1, 0, 0}};
		static constexpr MatrixRotationTemplate<T> Z_90 {Matrix<T, 3, 3>{ 0,-1, 0, 1, 0, 0, 0, 0, 1}};

		static constexpr MatrixRotationTemplate<T> X_180{Matrix<T, 3, 3>{ 1, 0, 0, 0,-1, 0, 0, 0,-1}};
		static constexpr MatrixRotationTemplate<T> Y_180{Matrix<T, 3, 3>{-1, 0, 0, 0, 1, 0, 0, 0,-1}};
		static constexpr MatrixRotationTemplate<T> Z_180{Matrix<T, 3, 3>{-1, 0, 0, 0,-1, 0, 0, 0, 1}};

		static constexpr MatrixRotationTemplate<T> X_270{Matrix<T, 3, 3>{ 1, 0, 0, 0, 0, 1, 0,-1, 0}};
		static constexpr MatrixRotationTemplate<T> Y_270{Matrix<T, 3, 3>{ 0, 0,-1, 0, 1, 0, 1, 0, 0}};
		static constexpr MatrixRotationTemplate<T> Z_270{Matrix<T, 3, 3>{ 0, 1, 0,-1, 0, 0, 0, 0, 1}};
	};
};

template<typename T>
class QuaternionRotationTemplate {
	Quaternion<T> rotationQuaternion;
	constexpr explicit QuaternionRotationTemplate(const Quaternion<T>& rotationQuaternion) : rotationQuaternion(rotationQuaternion) {};
public:

	constexpr QuaternionRotationTemplate();

	Vector<T, 3> localToGlobal(const Vector<T, 3>& vec) const;
	Vector<T, 3> globalToLocal(const Vector<T, 3>& vec) const;
	QuaternionRotationTemplate localToGlobal(const QuaternionRotationTemplate<T>& rot) const;
	QuaternionRotationTemplate globalToLocal(const QuaternionRotationTemplate<T>& rot) const;

	/*
		Transforms the given symmetric matrix into the basis of this rotation

		effectively brings a local matrix into the global space

		defined as   q * sm * ~q

		q.localToGlobal(sm * v) == q.localToGlobal(sm) * q.localToGlobal(v)
		== (q * sm * ~q) * q * v * ~q == (q * sm) * v * ~q == q * (sm * v) * ~q
	*/
	SymmetricMatrix<T, 3> localToGlobal(const SymmetricMatrix<T, 3>& sm) const;

	/*
		Transforms the given symmetric matrix from the basis of this rotation

		effectively brings a global matrix into the local space

		defined as   ~q * sm * q

		q.globalToLocal(sm * v) == q.globalToLocal(sm) * q.globalToLocal(v)
		== (~q * sm * q) * ~q * v * q == (~q * sm) * v * q == ~q * (sm * v) * q
	*/
	SymmetricMatrix<T, 3> globalToLocal(const SymmetricMatrix<T, 3>& sm) const;

	QuaternionRotationTemplate operator~() const;
	QuaternionRotationTemplate operator*(const QuaternionRotationTemplate& rot2) const;
	QuaternionRotationTemplate& operator*=(const QuaternionRotationTemplate& rot2);
	Vector<T, 3> operator*(const Vector<T, 3>& vec) const;
	
	/* returns the projection of the x-axis
		== this->localToGlobal(Vec3(1,0,0)) */
	Vector<T, 3> getX() const;
	/* returns the projection of the y-axis
		== this->localToGlobal(Vec3(0,1,0)) */
	Vector<T, 3> getY() const;
	/* returns the projection of the z-axis
		== this->localToGlobal(Vec3(0,0,1)) */
	Vector<T, 3> getZ() const;

	static QuaternionRotationTemplate rotX(T angle);
	static QuaternionRotationTemplate rotY(T angle);
	static QuaternionRotationTemplate rotZ(T angle);
	static QuaternionRotationTemplate fromEulerAngles(T alpha, T beta, T gamma) {
		return QuaternionRotationTemplate<T>::rotZ(gamma) * QuaternionRotationTemplate<T>::rotX(alpha) * QuaternionRotationTemplate<T>::rotY(beta);
	}
	static QuaternionRotationTemplate fromRotationVec(const Vector<T, 3>& rotationVector);
	static QuaternionRotationTemplate fromRotationMatrix(const Matrix<T, 3, 3>& rotationMatrix);
	static QuaternionRotationTemplate fromRotationQuaternion(const Quaternion<T>& rotationQuaternion);

	static QuaternionRotationTemplate faceX(Vector<T, 3> xDirection);
	static QuaternionRotationTemplate faceX(Vector<T, 3> xDirection, Vector<T, 3> yHint);
	static QuaternionRotationTemplate faceY(Vector<T, 3> yDirection);
	static QuaternionRotationTemplate faceY(Vector<T, 3> yDirection, Vector<T, 3> zHint);
	static QuaternionRotationTemplate faceZ(Vector<T, 3> zDirection);
	static QuaternionRotationTemplate faceZ(Vector<T, 3> zDirection, Vector<T, 3> xHint);

	Matrix<T, 3, 3> asRotationMatrix() const;
	Quaternion<T> asRotationQuaternion() const;
	Vector<T, 3> asRotationVector() const;

	/*operator Matrix<T, 3, 3>() const {
		return this->asRotationMatrix();
	}*/

	template<typename OtherT>
	operator QuaternionRotationTemplate<OtherT>() const;

	struct Predefined{
		static constexpr QuaternionRotationTemplate<T> IDENTITY{Quaternion<T>(1,0,0,0)};

		static constexpr QuaternionRotationTemplate<T> X_90 {Quaternion<T>(sq2_2<T>(), sq2_2<T>(), 0, 0)};
		static constexpr QuaternionRotationTemplate<T> Y_90 {Quaternion<T>(sq2_2<T>(), 0, sq2_2<T>(), 0)};
		static constexpr QuaternionRotationTemplate<T> Z_90 {Quaternion<T>(sq2_2<T>(), 0, 0, sq2_2<T>())};

		static constexpr QuaternionRotationTemplate<T> X_180{Quaternion<T>(0, 1, 0, 0)};
		static constexpr QuaternionRotationTemplate<T> Y_180{Quaternion<T>(0, 0, 1, 0)};
		static constexpr QuaternionRotationTemplate<T> Z_180{Quaternion<T>(0, 0, 0, 1)};

		static constexpr QuaternionRotationTemplate<T> X_270{Quaternion<T>(sq2_2<T>(), -sq2_2<T>(), 0, 0)};
		static constexpr QuaternionRotationTemplate<T> Y_270{Quaternion<T>(sq2_2<T>(), 0, -sq2_2<T>(), 0)};
		static constexpr QuaternionRotationTemplate<T> Z_270{Quaternion<T>(sq2_2<T>(), 0, 0, -sq2_2<T>())};
	};
};



template<typename T>
constexpr MatrixRotationTemplate<T>::MatrixRotationTemplate() : rotationMatrix(Matrix<T, 3, 3>::IDENTITY()) {}

template<typename T>
Vector<T, 3> MatrixRotationTemplate<T>::localToGlobal(const Vector<T, 3>& vec) const {
	return rotationMatrix * vec;
}

template<typename T>
Vector<T, 3> MatrixRotationTemplate<T>::globalToLocal(const Vector<T, 3>& vec) const {
	return rotationMatrix.transpose() * vec;
}

template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::localToGlobal(const MatrixRotationTemplate<T>& rot) const {
	return MatrixRotationTemplate<T>(this->rotationMatrix * rot.rotationMatrix);
}

template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::globalToLocal(const MatrixRotationTemplate<T>& rot) const {
	return MatrixRotationTemplate<T>(this->rotationMatrix.transpose() * rot.rotationMatrix);
}

template<typename T>
SymmetricMatrix<T, 3> MatrixRotationTemplate<T>::localToGlobal(const SymmetricMatrix<T, 3>& sm) const {
	return mulSymmetricLeftRightTranspose(sm, this->rotationMatrix);
}

template<typename T>
SymmetricMatrix<T, 3> MatrixRotationTemplate<T>::globalToLocal(const SymmetricMatrix<T, 3>& sm) const {
	return mulSymmetricLeftTransposeRight(sm, this->rotationMatrix);
}

template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::operator~() const {
	return MatrixRotationTemplate<T>(rotationMatrix.transpose());
}

template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::operator*(const MatrixRotationTemplate<T>& rot2) const {
	return this->localToGlobal(rot2);
}

template<typename T>
Vector<T, 3> MatrixRotationTemplate<T>::operator*(const Vector<T, 3>& vec) const {
	return this->localToGlobal(vec);
}

template<typename T>
Vector<T, 3> MatrixRotationTemplate<T>::getX() const {
	return this->rotationMatrix.getCol(0);
}

template<typename T>
Vector<T, 3> MatrixRotationTemplate<T>::getY() const {
	return this->rotationMatrix.getCol(1);
}

template<typename T>
Vector<T, 3> MatrixRotationTemplate<T>::getZ() const {
	return this->rotationMatrix.getCol(2);
}

template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::rotX(T angle) {
	return MatrixRotationTemplate<T>(::rotMatX(angle));
}
template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::rotY(T angle) {
	return MatrixRotationTemplate<T>(::rotMatY(angle));
}
template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::rotZ(T angle) {
	return MatrixRotationTemplate<T>(::rotMatZ(angle));
}


template<typename T>
Matrix<T, 3, 3> MatrixRotationTemplate<T>::asRotationMatrix() const {
	return this->rotationMatrix;
}
template<typename T>
Quaternion<T> MatrixRotationTemplate<T>::asRotationQuaternion() const {
	return rotationQuaternionFromRotationMatrix(this->rotationMatrix);
}
template<typename T>
Vector<T, 3> MatrixRotationTemplate<T>::asRotationVector() const {
	return rotationVectorFromRotationMatrix(this->rotationMatrix);
}

template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::fromRotationMatrix(const Matrix<T, 3, 3>& rotationMatrix) {
	assert(isValidRotationMatrix(rotationMatrix));
	return MatrixRotationTemplate<T>(rotationMatrix);
}
template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::fromRotationQuaternion(const Quaternion<T>& rotationQuaternion) {
	assert(isValidRotationQuaternion(rotationQuaternion));
	return MatrixRotationTemplate<T>(rotationMatrixFromQuaternion(rotationQuaternion));
}
template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::fromRotationVec(const Vector<T, 3>& rotationVector) {
	return MatrixRotationTemplate<T>(::rotationMatrixFromRotationVec(rotationVector));
}


template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::faceX(Vector<T, 3> xDirection) {
	return MatrixRotationTemplate<T>::fromRotationMatrix(::faceMatX(xDirection));
}
template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::faceX(Vector<T, 3> xDirection, Vector<T, 3> yHint) {
	return MatrixRotationTemplate<T>::fromRotationMatrix(::faceMatX(xDirection, yHint));
}
template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::faceY(Vector<T, 3> yDirection) {
	return MatrixRotationTemplate<T>::fromRotationMatrix(::faceMatY(yDirection));
}
template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::faceY(Vector<T, 3> yDirection, Vector<T, 3> zHint) {
	return MatrixRotationTemplate<T>::fromRotationMatrix(::faceMatY(yDirection, zHint));
}
template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::faceZ(Vector<T, 3> zDirection) {
	return MatrixRotationTemplate<T>::fromRotationMatrix(::faceMatZ(zDirection));
}
template<typename T>
MatrixRotationTemplate<T> MatrixRotationTemplate<T>::faceZ(Vector<T, 3> zDirection, Vector<T, 3> xHint) {
	return MatrixRotationTemplate<T>::fromRotationMatrix(::faceMatZ(zDirection, xHint));
}

template<typename T>
template<typename OtherT>
MatrixRotationTemplate<T>::operator MatrixRotationTemplate<OtherT>() const {
	return MatrixRotationTemplate<OtherT>::fromRotationMatrix(static_cast<Matrix<OtherT, 3, 3>>(this->rotationMatrix));
}



template<typename T>
constexpr QuaternionRotationTemplate<T>::QuaternionRotationTemplate() : rotationQuaternion(1, 0, 0, 0) {}

template<typename T>
Vector<T, 3> QuaternionRotationTemplate<T>::localToGlobal(const Vector<T, 3>& v) const {
	return mulQuaternionLeftRightConj(this->rotationQuaternion, v);
}

template<typename T>
Vector<T, 3> QuaternionRotationTemplate<T>::globalToLocal(const Vector<T, 3>& v) const {
	return mulQuaternionLeftConjRight(this->rotationQuaternion, v);
}

template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::localToGlobal(const QuaternionRotationTemplate<T>& rot) const {
	return QuaternionRotationTemplate<T>(this->rotationQuaternion * rot.rotationQuaternion);
}

template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::globalToLocal(const QuaternionRotationTemplate<T>& rot) const {
	return QuaternionRotationTemplate<T>(conj(this->rotationQuaternion) * rot.rotationQuaternion);
}

template<typename T>
SymmetricMatrix<T, 3> QuaternionRotationTemplate<T>::localToGlobal(const SymmetricMatrix<T, 3>& sm) const {
	SquareMatrix<T, 3> rotMat = this->asRotationMatrix();
	return mulSymmetricLeftRightTranspose(sm, rotMat);
}

template<typename T>
SymmetricMatrix<T, 3> QuaternionRotationTemplate<T>::globalToLocal(const SymmetricMatrix<T, 3>& sm) const {
	SquareMatrix<T, 3> rotMat = this->asRotationMatrix();
	return mulSymmetricLeftTransposeRight(sm, rotMat);
}

template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::operator~() const {
	return QuaternionRotationTemplate<T>(conj(rotationQuaternion));
}

template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::operator*(const QuaternionRotationTemplate<T>& rot2) const {
	return this->localToGlobal(rot2);
}

template<typename T>
Vector<T, 3> QuaternionRotationTemplate<T>::operator*(const Vector<T, 3>& vec) const {
	return this->localToGlobal(vec);
}

template<typename T>
Vector<T, 3> QuaternionRotationTemplate<T>::getX() const {
	T w = this->rotationQuaternion.w;
	Vector<T, 3> u = this->rotationQuaternion.v;
	
	return Vector<T, 3>(
		1 - 2 * (u.y * u.y + u.z * u.z),
		2 * (u.y * u.z + w * u.z),
		2 * (u.z * u.x - w * u.y)
	);
}

template<typename T>
Vector<T, 3> QuaternionRotationTemplate<T>::getY() const {
	T w = this->rotationQuaternion.w;
	Vector<T, 3> u = this->rotationQuaternion.v;
	
	return Vector<T, 3>(
		2 * (u.x * u.y - w * u.z),
		1 - 2 * (u.x * u.x + u.z * u.z),
		2 * (u.y * u.z + w * u.x)
	);
}

template<typename T>
Vector<T, 3> QuaternionRotationTemplate<T>::getZ() const {
	T w = this->rotationQuaternion.w;
	Vector<T, 3> u = this->rotationQuaternion.v;
	
	return Vector<T, 3>(
		2 * (u.x * u.z + w * u.y),
		2 * (u.y * u.z - w * u.x),
		1 - 2 * (u.x * u.x + u.y * u.y)
	);
}

template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::rotX(T angle) {
	return QuaternionRotationTemplate<T>(::rotQuatX(angle));
}
template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::rotY(T angle) {
	return QuaternionRotationTemplate<T>(::rotQuatY(angle));
}
template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::rotZ(T angle) {
	return QuaternionRotationTemplate<T>(::rotQuatZ(angle));
}


template<typename T>
Matrix<T, 3, 3> QuaternionRotationTemplate<T>::asRotationMatrix() const {
	return rotationMatrixFromQuaternion(this->rotationQuaternion);
}
template<typename T>
Quaternion<T> QuaternionRotationTemplate<T>::asRotationQuaternion() const {
	return this->rotationQuaternion;
}
template<typename T>
Vector<T, 3> QuaternionRotationTemplate<T>::asRotationVector() const {
	return rotationVectorFromRotationQuaternion(this->rotationQuaternion);
}

template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::fromRotationMatrix(const Matrix<T, 3, 3>& rotationMatrix) {
	assert(isValidRotationMatrix(rotationMatrix));
	return QuaternionRotationTemplate<T>(rotationQuaternionFromRotationMatrix(rotationMatrix));
}

template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::fromRotationQuaternion(const Quaternion<T>& rotationQuaternion) {
	assert(isValidRotationQuaternion(rotationQuaternion));
	return QuaternionRotationTemplate<T>(rotationQuaternion);
}

template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::fromRotationVec(const Vector<T, 3>& rotationVector) {
	return QuaternionRotationTemplate<T>(rotationQuaternionFromRotationVec(rotationVector));
}

template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::faceX(Vector<T, 3> xDirection) {
	return QuaternionRotationTemplate<T>::fromRotationMatrix(::faceMatX(xDirection));
}
template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::faceX(Vector<T, 3> xDirection, Vector<T, 3> yHint) {
	return QuaternionRotationTemplate<T>::fromRotationMatrix(::faceMatX(xDirection, yHint));
}
template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::faceY(Vector<T, 3> yDirection) {
	return QuaternionRotationTemplate<T>::fromRotationMatrix(::faceMatY(yDirection));
}
template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::faceY(Vector<T, 3> yDirection, Vector<T, 3> zHint) {
	return QuaternionRotationTemplate<T>::fromRotationMatrix(::faceMatY(yDirection, zHint));
}
template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::faceZ(Vector<T, 3> zDirection) {
	return QuaternionRotationTemplate<T>::fromRotationMatrix(::faceMatZ(zDirection));
}
template<typename T>
QuaternionRotationTemplate<T> QuaternionRotationTemplate<T>::faceZ(Vector<T, 3> zDirection, Vector<T, 3> xHint) {
	return QuaternionRotationTemplate<T>::fromRotationMatrix(::faceMatZ(zDirection, xHint));
}

template<typename T>
template<typename OtherT>
QuaternionRotationTemplate<T>::operator QuaternionRotationTemplate<OtherT>() const {
	return QuaternionRotationTemplate<OtherT>::fromRotationQuaternion(static_cast<Quaternion<OtherT>>(this->rotationQuaternion));
}


template<typename T>
using RotationTemplate = MatrixRotationTemplate<T>;
typedef RotationTemplate<double> Rotation;
typedef RotationTemplate<float> Rotationf;
