#include "physical.h"

#include "../util/log.h"
#include "math/mathUtil.h"

#include "debug.h"



Physical::Physical(Part p) : part(p) {
	this->mass = p.hitbox.getVolume() * p.properties.density;
	this->inertia = p.hitbox.getInertia() * p.properties.density;
}

// Physical::Physical(Part p, CFrame partNormalization, CFrame cframe, double mass, Vec3 centerOfMass, Mat3 inertia) : part(p), cframe(cframe), mass(mass), com(centerOfMass), inertia(inertia) {}

// Physical::Physical(NormalizedShape s, CFrame cframe, double density) : part(s, density, 0.1) {}

void Physical::update(double deltaT) {
	Vec3 accel = totalForce * (deltaT/mass);
	
	Vec3 localMoment = part.cframe.relativeToLocal(totalMoment);
	Vec3 localRotAcc = ~inertia * localMoment * deltaT;
	Vec3 rotAcc = part.cframe.localToRelative(localRotAcc);

	totalForce = Vec3();
	totalMoment = Vec3();

	velocity += accel;
	angularVelocity += rotAcc;

	Vec3 movement = velocity * deltaT;

	Mat3 rotation = fromRotationVec(angularVelocity * deltaT);

	part.cframe.translate(movement);
	part.cframe.rotate(rotation);
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
	return part.cframe.position;
}

Vec3 Physical::getVelocityOfPoint(Vec3Relative point) const {
	return velocity + angularVelocity % point;
}
