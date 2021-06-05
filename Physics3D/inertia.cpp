#include "inertia.h"

#include "math/linalg/trigonometry.h"
#include "math/predefinedTaylorExpansions.h"

namespace P3D {
SymmetricMat3 getRotatedInertia(const SymmetricMat3& originalInertia, const Rotation& rotation) {
	return rotation.localToGlobal(originalInertia);
}

SymmetricMat3 getTranslatedInertia(const SymmetricMat3& originalInertia, double mass, const Vec3& translation, const Vec3& centerOfMass) {
	SymmetricMat3 translationFactor = skewSymmetricSquared(translation + centerOfMass) - skewSymmetricSquared(centerOfMass);
	return originalInertia - translationFactor * mass;
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

SymmetricMat3 getTransformedInertia(const SymmetricMat3& originalInertia, double mass, const CFrame& cframe, const Vec3& centerOfMass) {
	return getTranslatedInertia(cframe.getRotation().localToGlobal(originalInertia), mass, cframe.getPosition(), centerOfMass);
}


/*
computes a translated inertial matrix, and it's derivatives
COMOffset is the offset of the object's center of mass from the resulting center of mass
motionOfOffset is the change of COMOffset over time, this is relative to the motion of the COM towards which this is computed
*/
FullTaylor<SymmetricMat3> getTranslatedInertiaDerivativesAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const Vec3& COMOffset, const TranslationalMotion& motionOfOffset) {
	FullTaylor<SymmetricMat3> result = -generateFullTaylorForSkewSymmetricSquared<double, 3>(FullTaylor<Vec3>::fromConstantAndDerivatives(COMOffset, motionOfOffset.translation));

	result *= mass;
	result += originalInertia;
	return result;
}

/*
computes a rotated inertial matrix, where originalInertia is the inertia around the center of mass of the transformed object
rotation is the starting rotation, and rotationMotion gives the change in rotation, both expressed in global space
*/
FullTaylor<SymmetricMat3> getRotatedInertiaTaylor(const SymmetricMat3& originalInertia, const Rotation& rotation, const RotationalMotion& rotationMotion) {
	Mat3 rotationMat = rotation.asRotationMatrix();
	Taylor<Mat3> rotationDerivs = generateTaylorForRotationMatrix<double, 2>(rotationMotion.rotation, rotationMat);


	// rotation(t) * originalInertia * rotation(t).transpose()
	// diff  => rotation(t) * originalInertia * rotation'(t).transpose() + rotation'(t) * originalInertia * rotation(t).transpose()
	// diff2 => 2 * rotation'(t) * originalInertia * rotation'(t).transpose() + rotation(t) * originalInertia * rotation''(t).transpose() + rotation''(t) * originalInertia * rotation(t).transpose()

	return FullTaylor<SymmetricMat3>{
		rotation.localToGlobal(originalInertia),
		addTransposed(rotationMat * originalInertia * rotationDerivs[0].transpose()),
		addTransposed(rotationMat * originalInertia * rotationDerivs[1].transpose()) + 2.0 * mulSymmetricLeftRightTranspose(originalInertia, rotationDerivs[0])};
}

/*
computes a transformed inertial matrix, where originalInertia is the inertia around the center of mass of the transformed object
localCenterOfMass is the center of mass of the transformed object
offsetCFrame is the offset of the object to it's new position relative to the total Center Of Mass
*/
SymmetricMat3 getTransformedInertiaAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const Vec3& localCenterOfMass, const CFrame& offsetCFrame) {
	Vec3 resultingOffset = offsetCFrame.localToGlobal(localCenterOfMass);
	return getTransformedInertiaAroundCenterOfMass(originalInertia, mass, CFrame(resultingOffset, offsetCFrame.getRotation()));
}

/*
computes a transformed inertial matrix, where originalInertia is the inertia around the center of mass of the transformed object
offsetCFrame is the offset of the object's center of mass and rotation relative to the COM of it's parent.
*/
SymmetricMat3 getTransformedInertiaAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const CFrame& offsetCFrame) {
	SymmetricMat3 translationFactor = skewSymmetricSquared(offsetCFrame.getPosition());
	return getRotatedInertia(originalInertia, offsetCFrame.getRotation()) - translationFactor * mass;
}

/*
computes the derivatives of a transformed inertial matrix, where originalInertia is the inertia around the center of mass of the transformed object
localCenterOfMass is the local center of mass in the local coordinate system of startingCFrame
startingCFrame is the current relative position
motion is the relative motion of the offset object's center of mass relative to the total center of mass, in the coordinate system of the total center of mass.
*/
FullTaylor<SymmetricMat3> getTransformedInertiaDerivativesAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const Vec3& localCenterOfMass, const CFrame& startingCFrame, const Motion& motion) {
	Vec3 relativeOffset = startingCFrame.localToRelative(localCenterOfMass);
	Vec3 resultingOffset = startingCFrame.getPosition() + relativeOffset;
	Motion resultingMotion = motion.getMotionOfPoint(relativeOffset);
	return getTransformedInertiaDerivativesAroundCenterOfMass(originalInertia, mass, CFrame(resultingOffset, startingCFrame.getRotation()), resultingMotion);
}

/*
computes a transformed inertial matrix, where originalInertia is the inertia around the center of mass of the transformed object
startingCFrame is the current relative position
motion is the relative motion of the offset object's center of mass relative to the total center of mass, in the coordinate system of the total center of mass.
*/
FullTaylor<SymmetricMat3> getTransformedInertiaDerivativesAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const CFrame& startingCFrame, const Motion& motion) {
	FullTaylor<Vec3> translation = FullTaylor<Vec3>::fromConstantAndDerivatives(startingCFrame.getPosition(), motion.translation.translation);
	Rotation originalRotation = startingCFrame.getRotation();
	RotationalMotion rotationMotion = motion.rotation;
	
	FullTaylor<SymmetricMat3> translationFactor = -generateFullTaylorForSkewSymmetricSquared<double, 3>(translation) * mass;
	//translationFactor.constantValue += originalInertia;

	FullTaylor<SymmetricMat3> rotationFactor = getRotatedInertiaTaylor(originalInertia, originalRotation, rotationMotion);

	return translationFactor + rotationFactor;
}
};