#pragma once

#include "math/linalg/mat.h"
#include "math/cframe.h"
#include "math/taylorExpansion.h"
#include "motion.h"
#include "relativeMotion.h"

namespace P3D {
SymmetricMat3 getRotatedInertia(const SymmetricMat3& originalInertia, const Rotation& rotation);
SymmetricMat3 getTranslatedInertia(const SymmetricMat3& originalInertia, double mass, const Vec3& translation, const Vec3& centerOfMass);
SymmetricMat3 getTransformedInertia(const SymmetricMat3& originalInertia, double mass, const CFrame& cframe, const Vec3& centerOfMass);

/*
computes a translated inertial matrix,
COMOffset is the offset of the object's center of mass from the resulting center of mass
== localCenterOfMass - totalCenterOfMass
*/
SymmetricMat3 getTranslatedInertiaAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const Vec3& COMOffset);

/*
computes a transformed inertial matrix, where originalInertia is the inertia around the center of mass of the transformed object
offsetCFrame is the offset of the object's center of mass and rotation relative to the COM of it's parent.
*/
SymmetricMat3 getTransformedInertiaAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const CFrame& offsetCFrame);

/*
computes a transformed inertial matrix, where originalInertia is the inertia around the center of mass of the transformed object
localCenterOfMass is the center of mass of the transformed object
offsetCFrame is the offset of the object to it's new position relative to the total Center Of Mass
*/
SymmetricMat3 getTransformedInertiaAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const Vec3& localCenterOfMass, const CFrame& offsetCFrame);

/*
computes a transformed inertial matrix, where originalInertia is the inertia around the center of mass of the transformed object
offsetCFrame is the offset of the object's center of mass and rotation relative to the COM of it's parent.
*/
SymmetricMat3 getTransformedInertiaAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const CFrame& offsetCFrame);

/*
computes a rotated inertial matrix, where originalInertia is the inertia around the center of mass of the transformed object
rotation is the starting rotation, and rotationMotion gives the change in rotation, both expressed in global space
*/
FullTaylor<SymmetricMat3> getRotatedInertiaTaylor(const SymmetricMat3& originalInertia, const Rotation& rotation, const RotationalMotion& rotationMotion);

/*
computes a translated inertial matrix, and it's derivatives
COMOffset is the offset of the object's center of mass from the resulting center of mass
motionOfOffset is the change of COMOffset over time, this is relative to the motion of the COM towards which this is computed
*/
FullTaylor<SymmetricMat3> getTranslatedInertiaDerivativesAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const Vec3& COMOffset, const TranslationalMotion& motionOfOffset);

/*
computes a transformed inertial matrix, where originalInertia is the inertia around the center of mass of the transformed object
startingCFrame is the current relative position
motion is the relative motion of the offset object's center of mass relative to the total center of mass, in the coordinate system of the total center of mass.
*/
FullTaylor<SymmetricMat3> getTransformedInertiaDerivativesAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const CFrame& startingCFrame, const Motion& motion);

/*
computes the derivatives of a transformed inertial matrix, where originalInertia is the inertia around the center of mass of the transformed object
localCenterOfMass is the local center of mass in the local coordinate system of startingCFrame
startingCFrame is the current relative position
motion is the relative motion of the offset object's center of mass relative to the total center of mass, in the coordinate system of the total center of mass.
*/
FullTaylor<SymmetricMat3> getTransformedInertiaDerivativesAroundCenterOfMass(const SymmetricMat3& originalInertia, double mass, const Vec3& localCenterOfMass, const CFrame& startingCFrame, const Motion& motion);


inline Vec3 getAngularMomentumFromOffsetOnlyVelocity(const Vec3& offset, const Vec3& velocity, double mass) {
	return offset % velocity * mass;
}


// offsetInertia = originalInertia - [ComOffset]x ^ 2 * mass
// angularMomemtum = (offsetInertia - [ComOffset]x ^ 2 * mass) * angularVel
// == offsetInertia * angularVel - ComOffset % (ComOffset % angularVel) * mass
// == offsetInertia * (velocity % ComOffset)/|ComOffset|^2 + ComOffset % velocity * mass
// leftOverAngularMomentum = 

inline Vec3 getAngularMomentumFromOffset(const Vec3& offset, const Vec3& velocity, const Vec3& angularVelocity, const SymmetricMat3& inertia, double mass) {
	Vec3 velocityAngularMomentum = getAngularMomentumFromOffsetOnlyVelocity(offset, velocity, mass);
	Vec3 rotationAngularMomentum = inertia * angularVelocity;
	return velocityAngularMomentum + rotationAngularMomentum;
}
};