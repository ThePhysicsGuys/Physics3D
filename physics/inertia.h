#pragma once

#include "math/linalg/mat.h"
#include "math/cframe.h"
#include "math/linalg/misc.h"

inline SymmetricMat3 getRotatedInertia(const SymmetricMat3& originalInertia, const RotMat3& rotation) {
	return transformBasis(originalInertia, rotation);
}
inline SymmetricMat3 getTranslatedInertia(const SymmetricMat3& originalInertia, const Vec3& translation, const Vec3& centerOfMass, double mass) {
	SymmetricMat3 translationFactor = skewSymmetricSquared(translation + centerOfMass) - skewSymmetricSquared(centerOfMass);
	return originalInertia + translationFactor * mass;
}
inline SymmetricMat3 getTransformedInertia(const SymmetricMat3& originalInertia, const CFrame& cframe, const Vec3& centerOfMass, double mass) {
	return getTranslatedInertia(transformBasis(originalInertia, cframe.getRotation()), cframe.getPosition(), centerOfMass, mass);
}
/* gets a translated inertial matrix, 
newCenterOfMass is the center around which the new inertia must be calculated
offsetCFrame is the offset of the object to it's new position*/
inline SymmetricMat3 getTranslatedInertiaAroundCenterOfMass(const SymmetricMat3& originalInertia, const Vec3& localCenterOfMass, const Vec3& totalCenterOfMass, double mass) {
	Vec3 resultingOffset = localCenterOfMass - totalCenterOfMass;
	SymmetricMat3 translationFactor = skewSymmetricSquared(resultingOffset);
	return originalInertia + translationFactor * mass;
}
/* gets a transformed inertial matrix, where originalInertia is the inertia around the center of mass of the transformed object
newCenterOfMass is the center around which the new inertia must be calculated
offsetCFrame is the offset of the object to it's new position*/
inline SymmetricMat3 getTransformedInertiaAroundCenterOfMass(const SymmetricMat3& originalInertia, const Vec3& localCenterOfMass, const CFrame& offsetCFrame, const Vec3& totalCenterOfMass, double mass) {
	Vec3 resultingOffset = offsetCFrame.localToGlobal(localCenterOfMass) - totalCenterOfMass;
	SymmetricMat3 translationFactor = skewSymmetricSquared(resultingOffset);
	return transformBasis(originalInertia, offsetCFrame.getRotation()) + translationFactor * mass;
}