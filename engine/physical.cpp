#include "physical.h"

#include "../util/log.h"
#include "math/mathUtil.h"

#include "debug.h"



Physical::Physical(Part* part) : cframe(part->cframe) {
	/*this->mass = part->hitbox.getVolume() * part->properties.density;
	SymmetricMat3 iner = part->hitbox.getInertia();
	//if(abs(iner.m01) > 1E-10 || abs(iner.m02) > 1E-10 || abs(iner.m12) > 1E-10) Log::error("inertia of normalized hitbox is not diagonal!");
	//DiagonalMat3 diagIner = DiagonalMat3(iner.m00, iner.m11, iner.m22);
	this->inertia = iner * part->properties.density;*/

	if (part->parent != nullptr) {
		throw "Attempting to re-add part to different physical!";
	}

	this->mass = part->mass;
	this->inertia = part->inertia;
	this->localCenterOfMass = Vec3(0, 0, 0);
	parts.push_back(AttachedPart{ CFrame(), part });
	part->parent = this;
}

void Physical::attachPart(Part* part, CFrame attachment) {
	parts.push_back(AttachedPart{attachment, part});

	part->parent = this;

	part->cframe = getCFrame().localToGlobal(attachment);

	refreshWithNewParts();
}
void Physical::detachPart(Part* part) {
	for (auto iter = parts.begin(); iter != parts.end(); ++iter) {
		AttachedPart& at = *iter;
		if (at.part == part) {
			at.part->parent = nullptr;
			parts.erase(iter);
			refreshWithNewParts();
			return;
		}
	}

	throw "Error: could not find part to remove!";
}

void Physical::refreshWithNewParts() {
	double totalMass = 0;
	SymmetricMat3 totalInertia(0, 0, 0, 0, 0, 0);
	Vec3 totalCenterOfMass(0, 0, 0);
	for (const AttachedPart& p : parts) {
		totalMass += p.part->mass;
		totalCenterOfMass += p.attachment.getPosition() * p.part->mass;
	}
	totalCenterOfMass /= totalMass;
	for (const AttachedPart& p : parts) {
		totalInertia += transformBasis(p.part->inertia, p.attachment.getRotation()) + skewSymmetricSquared(p.attachment.getPosition() - totalCenterOfMass) * p.part->mass;
	}
	this->mass = totalMass;
	this->localCenterOfMass = totalCenterOfMass;
	this->inertia = totalInertia;
}

void Physical::rotateAroundCenterOfMass(const RotMat3& rotation) {
	Vec3 relCenterOfMass = getCFrame().localToRelative(localCenterOfMass);
	Vec3 relativeRotationOffset = rotation * relCenterOfMass - relCenterOfMass;
	getCFrame().rotate(rotation);
	getCFrame().translate(-relativeRotationOffset);
}

void Physical::update(double deltaT) {
	Vec3 accel = totalForce * (deltaT/mass);
	
	Vec3 localMoment = cframe.relativeToLocal(totalMoment);
	Vec3 localRotAcc = ~inertia * localMoment * deltaT;
	Vec3 rotAcc = cframe.localToRelative(localRotAcc);

	totalForce = Vec3();
	totalMoment = Vec3();

	velocity += accel;
	angularVelocity += rotAcc;

	Vec3 movement = velocity * deltaT + accel * deltaT * deltaT / 2;

	Mat3 rotation = fromRotationVec(angularVelocity * deltaT);

	rotateAroundCenterOfMass(rotation);
	cframe.translate(movement);

	for (AttachedPart& attachment : parts) {
		attachment.part->cframe = cframe.localToGlobal(attachment.attachment);
	}
}

void Physical::applyForceAtCenterOfMass(Vec3 force) {
	totalForce += force;

	Debug::logVec(getCenterOfMass(), force, Debug::FORCE);
}

void Physical::applyForce(Vec3Relative origin, Vec3 force) {
	totalForce += force;

	Debug::logVec(origin + getCenterOfMass(), force, Debug::FORCE);

	applyMoment(origin % force);
}

void Physical::applyMoment(Vec3 moment) {
	totalMoment += moment;
	Debug::logVec(getCenterOfMass(), moment, Debug::MOMENT);
}

void Physical::applyImpulseAtCenterOfMass(Vec3 impulse) {
	Debug::logVec(getCenterOfMass(), impulse, Debug::IMPULSE);
	velocity += impulse / mass;
}
void Physical::applyImpulse(Vec3Relative origin, Vec3Relative impulse) {
	Debug::logVec(origin + getCenterOfMass(), impulse, Debug::IMPULSE);
	velocity += impulse / mass;
	Vec3 angularImpulse = origin % impulse;
	applyAngularImpulse(angularImpulse);
}
void Physical::applyAngularImpulse(Vec3 angularImpulse) {
	Debug::logVec(getCenterOfMass(), angularImpulse, Debug::ANGULAR_IMPULSE);
	Vec3 localAngularImpulse = cframe.relativeToLocal(angularImpulse);
	Vec3 localRotAcc = ~inertia * localAngularImpulse;
	Vec3 rotAcc = cframe.localToRelative(localRotAcc);
	angularVelocity += rotAcc;
}

Vec3 Physical::getCenterOfMass() const {
	return cframe.localToGlobal(localCenterOfMass);
}

Vec3 Physical::getVelocityOfPoint(const Vec3Relative& point) const {
	return velocity + angularVelocity % point;
}

Vec3 Physical::getAcceleration() const {
	return totalForce / mass;
}

Vec3 Physical::getAngularAcceleration() const {
	return ~inertia * cframe.relativeToLocal(totalMoment);
}

Vec3 Physical::getAccelerationOfPoint(const Vec3Relative& point) const {
	return getAcceleration() + getAngularAcceleration() % point;
}

void Physical::setCFrame(const CFrame& newCFrame) {
	this->cframe = newCFrame;
	for (const AttachedPart& p : parts) {
		p.part->cframe = newCFrame.localToGlobal(p.attachment);
	}
}

/*
	Computes the force->acceleration transformation matrix
	Such that:
	a = M*F
*/
SymmetricMat3 Physical::getPointAccelerationMatrix(const Vec3Local& r) const {
	DiagonalMat3 movementFactor(1 / mass, 1 / mass, 1 / mass);

	Mat3 crossMat = createCrossProductEquivalent(r);

	SymmetricMat3 invInertia = ~inertia;

	SymmetricMat3 rotationFactor = multiplyLeftRight(invInertia , crossMat);

	return movementFactor + rotationFactor;
}
double Physical::getInertiaOfPointInDirectionLocal(const Vec3Local& localPoint, const Vec3Local& localDirection) const {
	SymmetricMat3 accMat = getPointAccelerationMatrix(localPoint);

	Vec3 force = localDirection;
	Vec3 accel = accMat * force;
	double accelInForceDir = accel * localDirection / localDirection.lengthSquared();

	return 1 / accelInForceDir;

	/*SymmetricMat3 accelToForceMat = ~accMat;
	Vec3 imaginaryForceForAcceleration = accelToForceMat * direction;
	double forcePerAccelRatio = imaginaryForceForAcceleration * direction / direction.lengthSquared();
	return forcePerAccelRatio;*/
}

double Physical::getInertiaOfPointInDirectionRelative(const Vec3Relative& relPoint, const Vec3Relative& relDirection) const {
	return getInertiaOfPointInDirectionLocal(getCFrame().relativeToLocal(relPoint), getCFrame().relativeToLocal(relDirection));
}

double Physical::getVelocityKineticEnergy() const {
	return mass * velocity.lengthSquared() / 2;
}
double Physical::getAngularKineticEnergy() const {
	Vec3 localAngularVel = cframe.relativeToLocal(angularVelocity);
	return (inertia * localAngularVel) * localAngularVel / 2;
}
double Physical::getKineticEnergy() const {
	return getVelocityKineticEnergy() + getAngularKineticEnergy();
}
