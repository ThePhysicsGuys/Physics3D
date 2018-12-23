#include "physical.h"

#include "../util/log.h"
#include "math/mathUtil.h"

#include "debug.h"

Physical::Physical(Part p, CFrame cframe) : part(p), cframe(cframe), mass(p.hitbox.getVolume() * p.properties.density), 
											inertia(p.hitbox.getInertia() * p.properties.density), com(p.hitbox.getCenterOfMass()) {}

void Physical::update(double deltaT) {
	Vec3 accel = totalForce * (deltaT/mass);
	
	Vec3 localMoment = ~cframe.rotation * totalMoment;
	Vec3 localRotAcc = ~inertia * localMoment * deltaT;
	Vec3 rotAcc = cframe.rotation * localRotAcc;

	//Vec3 rotAcc = totalMoment * deltaT;

	totalForce = Vec3();
	totalMoment = Vec3();

	velocity += accel;
	angularVelocity += rotAcc;

	Vec3 movement = velocity * deltaT;
	
	Mat3 rotation = fromRotationVec(angularVelocity * deltaT);

	Vec3 relCOM = cframe.rotation * com;

	cframe.translate(movement + (relCOM - rotation * relCOM));
	cframe.rotate(rotation);
}

void Physical::applyForceAtCenterOfMass(Vec3 force) {
	totalForce += force;

	Debug::logVec(getCenterOfMass(), force, Debug::FORCE);
}

void Physical::applyForce(Vec3Relative origin, Vec3 force) {
	totalForce += force;

	Debug::logVec(origin + getCenterOfMass(), force, Debug::FORCE);

	applyMoment(force % origin);
}

void Physical::applyMoment(Vec3 moment) {
	totalMoment += moment;
	Debug::logVec(getCenterOfMass(), moment, Debug::MOMENT);
}

Vec3 Physical::getCenterOfMass() {
	return cframe.localToGlobal(com);
}

Vec3 Physical::getVelocityOfPoint(Vec3Relative point) {
	return velocity + point % angularVelocity;
}
