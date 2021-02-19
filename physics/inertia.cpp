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
template<int Derivs>
TaylorExpansion<SymmetricMat3, Derivs> getRotatedInertiaTaylorTemplate(const SymmetricMat3& globalInertia, const TaylorExpansion<Vec3, Derivs>& rotationMotion) {
	TaylorExpansion<Mat3, Derivs> rotationDerivs = generateRotationTaylor<double, Derivs>(rotationMotion);

	TaylorExpansion<SymmetricMat3, Derivs> result;


	// rotation(t) * originalInertia * rotation(t).transpose()
	// diff  => rotation(t) * originalInertia * rotation'(t).transpose() + rotation'(t) * originalInertia * rotation(t).transpose()
	// diff2 => 2 * rotation'(t) * originalInertia * rotation'(t).transpose() + rotation(t) * originalInertia * rotation''(t).transpose() + rotation''(t) * originalInertia * rotation(t).transpose()

	if constexpr(Derivs >= 1) {
		result[0] = addTransposed(globalInertia * rotationDerivs[0].transpose());
		if constexpr(Derivs >= 2) {
			result[1] = addTransposed(globalInertia * rotationDerivs[1].transpose()) + 2.0 * mulSymmetricLeftRightTranspose(globalInertia, rotationDerivs[0]);
			if constexpr(Derivs >= 3) {
				throw "TODO add more derivatives to getRotatedInertiaTaylor";
			}
		}
	}

	return result;
}

/*
computes a rotated inertial matrix, where originalInertia is the inertia around the center of mass of the transformed object
rotation is the starting rotation, and rotationMotion gives the change in rotation, both expressed in global space
*/
FullTaylor<SymmetricMat3> getRotatedInertiaTaylor(const SymmetricMat3& originalInertia, const Rotation& rotation, const RotationalMotion& rotationMotion) {
	SymmetricMat3 globalInertia = rotation.localToGlobal(originalInertia);
	return FullTaylor<SymmetricMat3>::fromConstantAndDerivatives(globalInertia, getRotatedInertiaTaylorTemplate<2>(globalInertia, rotationMotion.rotation));
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

	FullTaylor<SymmetricMat3> rotationFactor = getRotatedInertiaTaylor(originalInertia, originalRotation, rotationMotion.rotation);

	return translationFactor + rotationFactor;
}

template<int Derivs>
FullTaylorExpansion<Vec3, Derivs> getRotationAngularMomentumDerivs(const SymmetricMat3& globalInertia, const TaylorExpansion<Vec3, Derivs>& rotation) {
	// d(inertia * angularVelocity)/dt = inertia' * angularVelocity + inertia * angularAcceleration
	TaylorExpansion<SymmetricMat3, Derivs - 1> inertiaTaylor = getRotatedInertiaTaylorTemplate<Derivs - 1>(globalInertia, rotation.getSubTaylor<Derivs - 1>());
	FullTaylorExpansion<SymmetricMat3, Derivs> fullInertiaTaylor = FullTaylorExpansion<SymmetricMat3, Derivs>::fromConstantAndDerivatives(globalInertia, inertiaTaylor);
	return derivsOfMultiplication(fullInertiaTaylor, rotation.asFullTaylor());
}

// we lose one derivative because angular momentum is already in the velocity domain, but this is no problem, as we only need this derivative to compute acceleration terms
FullTaylorExpansion<Vec3, 2> getRotationAngularMomentumDerivsOnlyRotation(const SymmetricMat3& globalInertia, const TaylorExpansion<Vec3, 2>& rotation) {
	return getRotationAngularMomentumDerivs(globalInertia, rotation);
}

// we lose one derivative because angular momentum is already in the velocity domain, but this is no problem, as we only need this derivative to compute acceleration terms
FullTaylorExpansion<Vec3, 2> getAngularMomentumDerivativesFromOffset(const Vec3& offset, const Motion& motion, const SymmetricMat3& inertia, double mass) {
	return getAngularMomentumDerivativesFromOffsetOnlyVelocity(offset, motion.translation.translation, mass) + getRotationAngularMomentumDerivs(inertia, motion.rotation.rotation);
}
