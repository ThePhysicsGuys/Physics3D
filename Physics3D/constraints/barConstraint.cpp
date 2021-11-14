#include "barConstraint.h"
#include "constraintImpl.h"

namespace P3D {
int BarConstraint::maxNumberOfParameters() const {
	return 1;
}

static ConstraintMatrixPair<1> makeMatrices(const PhysicalInfo& phys, const Vec3& attach, const Vec3& barAxis) {
	Vec3 attachRelativeToCOM = phys.cframe.localToRelative(attach) - phys.relativeCenterOfMass;

	Mat3 crossEquivAttach = createCrossProductEquivalent(attachRelativeToCOM);

	Matrix<double, 6, 1> parameterToMotion = Matrix<double, 6, 1>::fromColumns({join(phys.forceResponse * barAxis, phys.momentResponse * crossEquivAttach * barAxis)});
	Matrix<double, 1, 6> motionToEquation = Matrix<double, 1, 6>::fromRows({join(barAxis, (attachRelativeToCOM % barAxis))});

	return ConstraintMatrixPair<1>{parameterToMotion, motionToEquation};
}

ConstraintMatrixPack BarConstraint::getMatrices(const PhysicalInfo& physA, const PhysicalInfo& physB, double* matrixBuf, double* errorBuf) const {
	Vec3 barAxis = physB.cframe.localToGlobal(attachB) - physA.cframe.localToGlobal(attachA);

	ConstraintMatrixPair<1> cA = makeMatrices(physA, attachA, barAxis);
	ConstraintMatrixPair<1> cB = makeMatrices(physB, attachB, barAxis);

	double error0 = length(barAxis) - this->barLength;

	double velocityA = cA.motionToEquation * physA.motion.getDerivAsVec6(0);
	double velocityB = cB.motionToEquation * physB.motion.getDerivAsVec6(0);
	double error1 = velocityB - velocityA;

	Matrix<double, 1, NUMBER_OF_ERROR_DERIVATIVES> error = Matrix<double, 1, NUMBER_OF_ERROR_DERIVATIVES>::fromColumns({error0, error1});

	/*inMemoryMatrixVectorMultiply(motionToEquationMatrixA, getMotionVecForDeriv(physA.mainPhysical->motionOfCenterOfMass, 1), mA);
	inMemoryMatrixVectorMultiply(motionToEquationMatrixB, getMotionVecForDeriv(physB.mainPhysical->motionOfCenterOfMass, 1), mB);
	mB -= mA;
	errorValue.setCol(2, mB);*/

	return ConstraintMatrixPack(matrixBuf, errorBuf, cA, cB, error);
}
};