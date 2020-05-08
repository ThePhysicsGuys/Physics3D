#include "inertia.h"

#include "math/linalg/misc.h"

SymmetricMat3 getRotatedInertia(const SymmetricMat3& originalInertia, const Rotation& rotation) {
	return rotation.localToGlobal(originalInertia);
}
SymmetricMat3 getTranslatedInertia(const SymmetricMat3& originalInertia, double mass, const Vec3& translation, const Vec3& centerOfMass) {
	SymmetricMat3 translationFactor = skewSymmetricSquared(translation + centerOfMass) - skewSymmetricSquared(centerOfMass);
	return originalInertia + translationFactor * mass;
}
SymmetricMat3 getTransformedInertia(const SymmetricMat3& originalInertia, double mass, const CFrame& cframe, const Vec3& centerOfMass) {
	return getTranslatedInertia(cframe.getRotation().localToGlobal(originalInertia), mass, cframe.getPosition(), centerOfMass);
}
/*
computes a translated inertial matrix,
COMOffset is the offset of the object's center of mass from the resulting center of mass
*/
SymmetricMat3 getTranslatedInertiaAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const Vec3& COMOffset) {
	SymmetricMat3 translationFactor = skewSymmetricSquared(COMOffset);
	return originalInertia + translationFactor * mass;
}

/* computes a transformed inertial matrix, where originalInertia is the inertia around the center of mass of the transformed object
newCenterOfMass is the center around which the new inertia must be calculated
offsetCFrame is the offset of the object to it's new position*/
SymmetricMat3 getTransformedInertiaAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const Vec3& localCenterOfMass, const CFrame& offsetCFrame, const Vec3& totalCenterOfMass) {
	Vec3 resultingOffset = offsetCFrame.localToGlobal(localCenterOfMass) - totalCenterOfMass;
	SymmetricMat3 translationFactor = skewSymmetricSquared(resultingOffset);
	return offsetCFrame.getRotation().localToGlobal(originalInertia) + translationFactor * mass;
}



/*
computes a translated inertial matrix, and it's derivatives
COMOffset is the offset of the object's center of mass from the resulting center of mass
motionOfOffset is the change of COMOffset over time, this is relative to the motion of the COM towards which this is computed
*/
FullTaylor<SymmetricMat3> getTranslatedInertiaDerivativesAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const Vec3& COMOffset, const TranslationalMotion& motionOfOffset) {
	FullTaylor<SymmetricMat3> result{skewSymmetricSquared(COMOffset)};
	for(int i = 0; i < result.derivatives.size(); i++) {
		result.derivatives[i] = skewSymmetricSquared(motionOfOffset.translation.derivatives[i]);
	}
	result *= mass;
	result.constantValue += originalInertia;
	return result;
}
