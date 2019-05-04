#include "physical.h"

#include "../util/log.h"
#include "math/mathUtil.h"

#include "debug.h"



Physical::Physical(Part* part) : part(part) {
	this->mass = part->hitbox.getVolume() * part->properties.density;
	SymmetricMat3 iner = part->hitbox.getInertia();
	if(abs(iner.m01) > 1E-10 || abs(iner.m02) > 1E-10 || abs(iner.m12) > 1E-10) Log::error("inertia of normalized hitbox is not diagonal!");
	DiagonalMat3 diagIner = DiagonalMat3(iner.m00, iner.m11, iner.m22);
	this->inertia = diagIner * part->properties.density;
}

// Physical::Physical(Part p, CFrame partNormalization, CFrame cframe, double mass, Vec3 centerOfMass, Mat3 inertia) : part(p), cframe(cframe), mass(mass), com(centerOfMass), inertia(inertia) {}

// Physical::Physical(NormalizedShape s, CFrame cframe, double density) : part(s, density, 0.1) {}

void Physical::update(double deltaT) {
	Vec3 accel = totalForce * (deltaT/mass);
	
	Vec3 localMoment = part->cframe.relativeToLocal(totalMoment);
	Vec3 localRotAcc = ~inertia * localMoment * deltaT;
	Vec3 rotAcc = part->cframe.localToRelative(localRotAcc);

	totalForce = Vec3();
	totalMoment = Vec3();

	velocity += accel;
	angularVelocity += rotAcc;

	Vec3 movement = velocity * deltaT;

	Mat3 rotation = fromRotationVec(angularVelocity * deltaT);

	part->cframe.translate(movement);
	part->cframe.rotate(rotation);
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

Vec3 Physical::getCenterOfMass() const {
	return part->cframe.position;
}

Vec3 Physical::getVelocityOfPoint(const Vec3Relative& point) const {
	return velocity + angularVelocity % point;
}

Vec3 Physical::getAcceleration() const {
	return totalForce / mass;
}

Vec3 Physical::getAngularAcceleration() const {
	return ~inertia * part->cframe.relativeToLocal(totalMoment);
}

Vec3 Physical::getAccelerationOfPoint(const Vec3Relative& point) const {
	return getAcceleration() + getAngularAcceleration() % point;
}

/*
	Computes the force->acceleration transformation matrix
	Such that:
	a = M*F
*/
SymmetricMat3 Physical::getPointAccelerationMatrix(const Vec3Local& r) const {
	double Ix = this->inertia.m00;
	double Iy = this->inertia.m11;
	double Iz = this->inertia.m22;

	Vec3 rSq = r.squared();
	
	SymmetricMat3 rotationFactor(rSq.z / Iy + rSq.y / Iz, rSq.x / Iz + rSq.z / Ix, rSq.y / Ix + rSq.x / Iy,
								 -r.x*r.y / Iz, -r.x*r.z / Iy, -r.y*r.z / Ix);
	DiagonalMat3 movementFactor(1 / mass, 1 / mass, 1 / mass);

	return rotationFactor + movementFactor;
}
double Physical::getInertiaOfPointInDirection(const Vec3Local& localPoint, const Vec3Local& direction) const {
	SymmetricMat3 accMat1 = getPointAccelerationMatrix(localPoint);

	SymmetricMat3 accelToForceMat = ~accMat1;
	Vec3 imaginaryForceForAcceleration = accelToForceMat * direction;
	double forcePerAccelRatio = imaginaryForceForAcceleration * direction / direction.lengthSquared();
	return forcePerAccelRatio;
}
