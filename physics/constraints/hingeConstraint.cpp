#include "hingeConstraint.h"

int HingeConstraint::maxNumberOfParameters() const {
	return 5;
}

static ConstraintMatrixPair<5> makeMatrices(const PhysicalInfo& phys, const Vec3& attach, const Vec3& p1, const Vec3& p2) {
	Vec3 attachRelativeToCOM = phys.cframe.localToRelative(attach) - phys.relativeCenterOfMass;

	Mat3 crossEquivAttach = createCrossProductEquivalent(attachRelativeToCOM);

	Matrix<double, 3, 5> impulseToMotion = joinHorizontal(Mat3::DIAGONAL(phys.forceResponse), Matrix<double, 3, 2>::ZEROS());
	Matrix<double, 3, 5> angularEffect = joinHorizontal(crossEquivAttach, Matrix<double, 3, 2>::fromColumns({p1, p2}));

	Matrix<double, 6, 5> parameterToMotion = joinVertical(impulseToMotion, phys.momentResponse * angularEffect);
	Matrix<double, 5, 6> motionToEquation = join(Mat3::IDENTITY(), -crossEquivAttach, Matrix<double, 2, 3>::ZEROS(), Matrix<double, 2, 3>::fromRows({p1, p2}));

	return ConstraintMatrixPair<5>{parameterToMotion, motionToEquation};
}

ConstraintMatrixPack HingeConstraint::getMatrices(const PhysicalInfo& physA, const PhysicalInfo& physB, double* matrixBuf, double* errorBuf) const {
	Vec3 localMainAxis = normalize(this->axisA);
	Vec3 localP1 = normalize(getPerpendicular(localMainAxis));
	Vec3 localP2 = normalize(localMainAxis % localP1);
	
	Vec3 localMainOffsetAxis = normalize(this->axisB);

	Vec3 mainAxis = physA.cframe.localToRelative(localMainAxis);
	Vec3 p1 = physA.cframe.localToRelative(localP1);
	Vec3 p2 = physA.cframe.localToRelative(localP2);
	Vec3 mainOffsetAxis = physB.cframe.localToRelative(localMainOffsetAxis);
	
	// rotationOffset will be in the plane defined by p1 and p2, as it is perpendicular to mainAxis
	Vec3 rotationOffset = mainAxis % mainOffsetAxis;
	double rotationOffsetP1 = p1 * rotationOffset;
	double rotationOffsetP2 = p2 * rotationOffset;

	ConstraintMatrixPair<5> cA = makeMatrices(physA, attachA, p1, p2);
	ConstraintMatrixPair<5> cB = makeMatrices(physB, attachB, p1, p2);

	Vec5 error0 = join(Vec3(physB.cframe.localToGlobal(attachB) - physA.cframe.localToGlobal(attachA)), Vec2(rotationOffsetP1, rotationOffsetP2));

	Vec5 velocityA = cA.motionToEquation * physA.motion.getDerivAsVec6(0);
	Vec5 velocityB = cB.motionToEquation * physB.motion.getDerivAsVec6(0);
	Vec5 error1 = velocityB - velocityA;

	Matrix<double, 5, NUMBER_OF_ERROR_DERIVATIVES> error = Matrix<double, 5, NUMBER_OF_ERROR_DERIVATIVES>::fromColumns({error0, error1});

	return ConstraintMatrixPack(matrixBuf, errorBuf, cA, cB, error);
}
