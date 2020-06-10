#include "inertia.h"

#include "math/linalg/trigonometry.h"
#include "math/predefinedTaylorExpansions.h"

SymmetricMat3 getRotatedInertia(const SymmetricMat3& originalInertia, const Rotation& rotation) {
	return rotation.localToGlobal(originalInertia);
}
SymmetricMat3 getTranslatedInertia(const SymmetricMat3& originalInertia, double mass, const Vec3& translation, const Vec3& centerOfMass) {
	SymmetricMat3 translationFactor = skewSymmetricSquared(translation + centerOfMass) - skewSymmetricSquared(centerOfMass);
	return originalInertia - translationFactor * mass;
}
SymmetricMat3 getTransformedInertia(const SymmetricMat3& originalInertia, double mass, const CFrame& cframe, const Vec3& centerOfMass) {
	return getTranslatedInertia(cframe.getRotation().localToGlobal(originalInertia), mass, cframe.getPosition(), centerOfMass);
}

FullTaylor<SymmetricMat3> getRotatedInertiaTaylor(const SymmetricMat3& originalInertia, const Rotation& rotation, const RotationalMotion& rotationMotion) {
	Mat3 rotationMat = rotation.asRotationMatrix();
	Taylor<Mat3> rotationDerivs = generateTaylorForRotationMatrix(rotationMotion.rotation, rotationMat);
	
	FullTaylor<SymmetricMat3> result(rotation.localToGlobal(originalInertia));

	// rotation(t) * originalInertia * rotation(t).transpose()
	// diff  => rotation'(t) * originalInertia * rotation(t).transpose() + rotation(t) * originalInertia * rotation'(t).transpose()
	// diff2 => rotation''(t) * originalInertia * rotation(t).transpose() + 2 * rotation'(t) * originalInertia * rotation'(t).transpose() + rotation(t) * originalInertia * rotation''(t).transpose()

	result.derivatives[0] = mulSymmetricSumRightTransposed(originalInertia, rotationMat, rotationDerivs[0]);
	result.derivatives[1] = mulSymmetricSumRightTransposed(originalInertia, rotationMat, rotationDerivs[1]) + 2.0*mulSymmetricLeftRightTranspose(originalInertia, rotationDerivs[0]);
	
	return result;
}

/*
computes a translated inertial matrix,
COMOffset is the offset of the object's center of mass from the resulting center of mass
== localCenterOfMass - totalCenterOfMass
*/
SymmetricMat3 getTranslatedInertiaAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const Vec3& COMOffset) {
	SymmetricMat3 translationFactor = skewSymmetricSquared(COMOffset);
	return originalInertia - translationFactor * mass;
}

/*
computes a translated inertial matrix, and it's derivatives
COMOffset is the offset of the object's center of mass from the resulting center of mass
motionOfOffset is the change of COMOffset over time, this is relative to the motion of the COM towards which this is computed
*/
FullTaylor<SymmetricMat3> getTranslatedInertiaDerivativesAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const Vec3& COMOffset, const TranslationalMotion& motionOfOffset) {
	FullTaylor<SymmetricMat3> result = -generateFullTaylorForSkewSymmetricSquared(FullTaylor<Vec3>{COMOffset, motionOfOffset.translation});
	
	result *= mass;
	result.constantValue += originalInertia;
	return result;
}

/* 
computes a transformed inertial matrix, where originalInertia is the inertia around the center of mass of the transformed object
totalCenterOfMass is the center around which the new inertia must be calculated
localCenterOfMass is the center of mass of the transformed object
offsetCFrame is the offset of the object to it's new position
*/
SymmetricMat3 getTransformedInertiaAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const Vec3& localCenterOfMass, const CFrame& offsetCFrame, const Vec3& totalCenterOfMass) {
	Vec3 resultingOffset = offsetCFrame.localToGlobal(localCenterOfMass) - totalCenterOfMass;
	SymmetricMat3 translationFactor = skewSymmetricSquared(resultingOffset);
	return getRotatedInertia(originalInertia, offsetCFrame.getRotation()) - translationFactor * mass;
}
