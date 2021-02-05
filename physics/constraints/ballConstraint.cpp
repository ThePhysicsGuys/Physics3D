#include "ballConstraint.h"

int BallConstraint::maxNumberOfParameters() const {
	return 3;
}

static ConstraintMatrixPair<3> makeMatrices(const PhysicalInfo& phys, const Vec3& attach) {
	Vec3 attachRelativeToCOM = phys.cframe.localToRelative(attach) - phys.relativeCenterOfMass;
	
	Mat3 crossEquivAttach = createCrossProductEquivalent(attachRelativeToCOM);

	Matrix<double, 6, 3> parameterToMotion = joinVertical(Mat3::IDENTITY() * phys.forceResponse, phys.momentResponse * crossEquivAttach);
	Matrix<double, 3, 6> motionToEquation = joinHorizontal(Mat3::IDENTITY(), -crossEquivAttach);
	
	return ConstraintMatrixPair<3>{parameterToMotion, motionToEquation};
}

ConstraintMatrixPack BallConstraint::getMatrices(const PhysicalInfo& physA, const PhysicalInfo& physB, double* matrixBuf, double* errorBuf) const {
	ConstraintMatrixPair<3> cA = makeMatrices(physA, attachA);
	ConstraintMatrixPair<3> cB = makeMatrices(physB, attachB);

	Vec3 error0 = physB.cframe.localToGlobal(attachB) - physA.cframe.localToGlobal(attachA);

	Vec3 velocityA = cA.motionToEquation * physA.motion.getDerivAsVec6(0);
	Vec3 velocityB = cB.motionToEquation * physB.motion.getDerivAsVec6(0);
	Vec3 error1 = velocityB - velocityA;

	Matrix<double, 3, NUMBER_OF_ERROR_DERIVATIVES> error = Matrix<double, 3, NUMBER_OF_ERROR_DERIVATIVES>::fromColumns({error0, error1});

	/*inMemoryMatrixVectorMultiply(motionToEquationMatrixA, getMotionVecForDeriv(physA.mainPhysical->motionOfCenterOfMass, 1), mA);
	inMemoryMatrixVectorMultiply(motionToEquationMatrixB, getMotionVecForDeriv(physB.mainPhysical->motionOfCenterOfMass, 1), mB);
	mB -= mA;
	errorValue.setCol(2, mB);*/

	return ConstraintMatrixPack(matrixBuf, errorBuf, cA, cB, error);
}
