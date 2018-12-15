#include "physical.h"

#include "../util/log.h"
#include "math/mathUtil.h"

#include "debug.h"

Physical::Physical(Part p, CFrame cframe, Mat3 inertia) : part(p), cframe(cframe), mass(p.hitbox.getVolume() * p.properties.density), inertia(inertia), com(p.hitbox.getCenterOfMass()) {}

void Physical::update(double deltaT) {
	Vec3 accel = totalForce * (deltaT/mass);
	Vec3 rotAcc = ~inertia * totalMoment * deltaT;

	totalForce = Vec3();
	totalMoment = Vec3();

	velocity += accel;
	angularVelocity += rotAcc;

	cframe = cframe + velocity * deltaT;
	
	Mat3 rotation = fromRotationVec(angularVelocity * deltaT);

	cframe.rotation = rotation * cframe.rotation;
}

void Physical::applyForceAtCenterOfMass(Vec3 force) {
	totalForce += force;

	Debug::logVec(getCenterOfMass(), force, Debug::FORCE);
}

void Physical::applyForce(Vec3Relative origin, Vec3 force) {
	totalForce += force;
	totalMoment += force % origin;

	Debug::logVec(origin + getCenterOfMass(), force, Debug::FORCE);
}

Vec3 Physical::getCenterOfMass() {
	return cframe.localToGlobal(com);
}

Vec3 Physical::getVelocityOfPoint(Vec3Relative point) {
	return velocity + point % angularVelocity;
}
