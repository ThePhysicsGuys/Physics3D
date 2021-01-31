#include "ballConstraint.h"

#include "../physical.h"

int BallConstraint::numberOfParameters() const {
	return 3;
}

static void makeMatrices(const Physical& phys, const Vec3& attach, UnmanagedVerticalFixedMatrix<double, 6>& parameterToMotionMatrix, UnmanagedHorizontalFixedMatrix<double, 6>& motionToEquationMatrix) {
	Vec3 attachRelativeToCOM = phys.localToMainCOMRelative(attach);
	
	Mat3 crossEquivAttach = createCrossProductEquivalent(attachRelativeToCOM);

	setSubMatrix(parameterToMotionMatrix, DiagonalMat3::IDENTITY() * (1 / phys.mainPhysical->totalMass), 0, 0);
	setSubMatrix(parameterToMotionMatrix, phys.mainPhysical->momentResponse * crossEquivAttach, 3, 0);

	setSubMatrix(motionToEquationMatrix, DiagonalMat3::IDENTITY(), 0, 0);
	setSubMatrix(motionToEquationMatrix, -crossEquivAttach, 0, 3);
}

static Vector<double, 6> getMotionVecForDeriv(const Motion& motion, int deriv) {
	return join(motion.translation.translation[deriv], motion.rotation.rotation[deriv]);
}

void BallConstraint::getMatrices(const Physical& physA, const Physical& physB, UnmanagedVerticalFixedMatrix<double, 6>& parameterToMotionMatrixA, UnmanagedVerticalFixedMatrix<double, 6>& parameterToMotionMatrixB, UnmanagedHorizontalFixedMatrix<double, 6>& motionToEquationMatrixA, UnmanagedHorizontalFixedMatrix<double, 6>& motionToEquationMatrixB, UnmanagedHorizontalFixedMatrix<double, NUMBER_OF_ERROR_DERIVATIVES>& errorValue) const {
	makeMatrices(physA, attachA, parameterToMotionMatrixA, motionToEquationMatrixA);
	makeMatrices(physB, attachB, parameterToMotionMatrixB, motionToEquationMatrixB);

	double bufA[3]; UnmanagedLargeVector<double> mA(bufA, 3);
	double bufB[3]; UnmanagedLargeVector<double> mB(bufB, 3);



	errorValue.setCol(0, Vec3(physB.getCFrame().localToGlobal(attachB) - physA.getCFrame().localToGlobal(attachA)));

	inMemoryMatrixVectorMultiply(motionToEquationMatrixA, getMotionVecForDeriv(physA.mainPhysical->motionOfCenterOfMass, 0), mA);
	inMemoryMatrixVectorMultiply(motionToEquationMatrixB, getMotionVecForDeriv(physB.mainPhysical->motionOfCenterOfMass, 0), mB);
	mB -= mA;
	errorValue.setCol(1, mB);

	/*inMemoryMatrixVectorMultiply(motionToEquationMatrixA, getMotionVecForDeriv(physA.mainPhysical->motionOfCenterOfMass, 1), mA);
	inMemoryMatrixVectorMultiply(motionToEquationMatrixB, getMotionVecForDeriv(physB.mainPhysical->motionOfCenterOfMass, 1), mB);
	mB -= mA;
	errorValue.setCol(2, mB);*/
}
