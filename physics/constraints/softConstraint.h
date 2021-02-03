#pragma once


#include "../math/linalg/mat.h"
#include "../math/linalg/largeMatrix.h"
#include "../math/globalCFrame.h"
#include "../motion.h"

#define NUMBER_OF_ERROR_DERIVATIVES 2

template<int Size>
struct ConstraintMatrixPair {
	Matrix<double, 6, Size> paramToMotion;
	Matrix<double, Size, 6> motionToEquation;

	ConstraintMatrixPair operator-() const {
		return ConstraintMatrixPair{-paramToMotion, -motionToEquation};
	}
};

struct PhysicalInfo {
	GlobalCFrame cframe;
	Motion motion;
	double forceResponse; // 1/mass
	Mat3 momentResponse;
	Vec3 relativeCenterOfMass;

	inline Vector<double, 6> getMotionVecForDeriv(int deriv) const {
		return join(motion.translation.translation[deriv], motion.rotation.rotation[deriv]);
	}
};

class ConstraintMatrixPack {
	double* matrixData;
	double* errorData;
	int size; // size * 6 for matrix size, size * 6 * 4 for total size

public:
	ConstraintMatrixPack() = default;
	template<int Size>
	ConstraintMatrixPack(double* matrixBuf, double* errorBuf, 
						 const Matrix<double, 6, Size>& paramToMotionA, 
						 const Matrix<double, 6, Size>& paramToMotionB, 
						 const Matrix<double, Size, 6>& motionToEqA, 
						 const Matrix<double, Size, 6>& motionToEqB, 
						 const Matrix<double, Size, NUMBER_OF_ERROR_DERIVATIVES>& errorMat) : matrixData(matrixBuf), errorData(errorBuf), size(Size) {

		paramToMotionA.toColMajorData(matrixData);
		paramToMotionB.toColMajorData(matrixData + 6 * Size);
		motionToEqA.toRowMajorData(matrixData + 12 * Size);
		motionToEqB.toRowMajorData(matrixData + 18 * Size);
		errorMat.toRowMajorData(errorData);
	}
	template<int Size>
	ConstraintMatrixPack(double* matrixBuf, double* errorBuf,
						 const ConstraintMatrixPair<Size>& mA,
						 const ConstraintMatrixPair<Size>& mB,
						 const Matrix<double, Size, NUMBER_OF_ERROR_DERIVATIVES>& errorMat) : matrixData(matrixBuf), errorData(errorBuf), size(Size) {

		mA.paramToMotion.toColMajorData(matrixData);
		mB.paramToMotion.toColMajorData(matrixData + 6 * Size);
		mA.motionToEquation.toRowMajorData(matrixData + 12 * Size);
		mA.motionToEquation.toRowMajorData(matrixData + 18 * Size);
		errorMat.toRowMajorData(errorData);
	}
	int getSize() const { return size; }
	UnmanagedVerticalFixedMatrix<double, 6> getParameterToMotionMatrixA() const {
		return UnmanagedVerticalFixedMatrix<double, 6>(matrixData, size);
	}
	UnmanagedVerticalFixedMatrix<double, 6> getParameterToMotionMatrixB() const {
		return UnmanagedVerticalFixedMatrix<double, 6>(matrixData + size_t(6) * size, size);
	}
	UnmanagedHorizontalFixedMatrix<double, 6> getMotionToEquationMatrixA() const {
		return UnmanagedHorizontalFixedMatrix<double, 6>(matrixData + size_t(12) * size, size);
	}
	UnmanagedHorizontalFixedMatrix<double, 6> getMotionToEquationMatrixB() const {
		return UnmanagedHorizontalFixedMatrix<double, 6>(matrixData + size_t(18) * size, size);
	}
	UnmanagedHorizontalFixedMatrix<double, NUMBER_OF_ERROR_DERIVATIVES> getErrorMatrix() const {
		return UnmanagedHorizontalFixedMatrix<double, NUMBER_OF_ERROR_DERIVATIVES>(errorData, size);
	}
};

struct Constraint {
	virtual int maxNumberOfParameters() const = 0;
	virtual ConstraintMatrixPack getMatrices(const PhysicalInfo& physA, const PhysicalInfo& physB, double* matrixBuf, double* errorBuf) const = 0;
};
