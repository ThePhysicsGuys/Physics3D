
#include "world.h"
#include "layer.h"

#include "math/mathUtil.h"
#include "math/linalg/vec.h"
#include "math/linalg/trigonometry.h"

#include "debug.h"
#include "constants.h"
#include "physicsProfiler.h"
#include "../util/log.h"

#include <vector>
#include <cmath>
#include <algorithm>

/*
	exitVector is the distance p2 must travel so that the shapes are no longer colliding
*/
void handleCollision(Part& part1, Part& part2, Position collisionPoint, Vec3 exitVector) {
	Debug::logPoint(collisionPoint, Debug::INTERSECTION);
	Physical& parent1 = *part1.parent;
	Physical& parent2 = *part2.parent;

	MotorizedPhysical& phys1 = *parent1.mainPhysical;
	MotorizedPhysical& phys2 = *parent2.mainPhysical;

	double sizeOrder = std::min(part1.maxRadius, part2.maxRadius);
	if(lengthSquared(exitVector) <= 1E-8 * sizeOrder*sizeOrder) {
		return; // don't do anything for very small colissions
	}

	Vec3 collissionRelP1 = collisionPoint - phys1.getCenterOfMass();
	Vec3 collissionRelP2 = collisionPoint - phys2.getCenterOfMass();

	double inertia1A = phys1.getInertiaOfPointInDirectionRelative(collissionRelP1, exitVector);
	double inertia2A = phys2.getInertiaOfPointInDirectionRelative(collissionRelP2, exitVector);
	double combinedInertia = 1 / (1 / inertia1A + 1 / inertia2A);

	// Friction
	double staticFriction = part1.properties.friction * part2.properties.friction;
	double dynamicFriction = part1.properties.friction * part2.properties.friction;

	
	Vec3 depthForce = -exitVector * (COLLISSION_DEPTH_FORCE_MULTIPLIER * combinedInertia);

	phys1.applyForce(collissionRelP1, depthForce);
	phys2.applyForce(collissionRelP2, -depthForce);

	Vec3 part1ToColission = collisionPoint - part1.getPosition();
	Vec3 part2ToColission = collisionPoint - part2.getPosition();

	Vec3 relativeVelocity = (part1.getMotion().getVelocityOfPoint(part1ToColission) - part1.properties.conveyorEffect) - (part2.getMotion().getVelocityOfPoint(part2ToColission) - part2.properties.conveyorEffect);

	bool isImpulseColission = relativeVelocity * exitVector > 0;

	Vec3 impulse;

	double combinedBouncyness = part1.properties.bouncyness * part2.properties.bouncyness;

	if(isImpulseColission) { // moving towards the other object
		Vec3 desiredAccel = -exitVector * (relativeVelocity * exitVector) / lengthSquared(exitVector) * (1.0 + combinedBouncyness);
		Vec3 zeroRelVelImpulse = desiredAccel * combinedInertia;
		impulse = zeroRelVelImpulse;
		phys1.applyImpulse(collissionRelP1, impulse);
		phys2.applyImpulse(collissionRelP2, -impulse);
		relativeVelocity += desiredAccel;
	}

	Vec3 slidingVelocity = exitVector % relativeVelocity % exitVector / lengthSquared(exitVector);

	// Compute combined inertia in the horizontal direction
	double inertia1B = phys1.getInertiaOfPointInDirectionRelative(collissionRelP1, slidingVelocity);
	double inertia2B = phys2.getInertiaOfPointInDirectionRelative(collissionRelP2, slidingVelocity);
	double combinedHorizontalInertia = 1 / (1 / inertia1B + 1 / inertia2B);

	if (isImpulseColission) {
		Vec3 maxFrictionImpulse = -exitVector % impulse % exitVector / lengthSquared(exitVector) * staticFriction;
		Vec3 stopFricImpulse = -slidingVelocity * combinedHorizontalInertia;

		Vec3 fricImpulse = (lengthSquared(stopFricImpulse) < lengthSquared(maxFrictionImpulse)) ? stopFricImpulse : maxFrictionImpulse;

		phys1.applyImpulse(collissionRelP1, fricImpulse);
		phys2.applyImpulse(collissionRelP2, -fricImpulse);
	}

	double normalForce = length(depthForce);
	double frictionForce = normalForce * dynamicFriction;
	double slidingSpeed = length(slidingVelocity) + 1E-100;
	Vec3 dynamicFricForce;
	double dynamicSaturationSpeed = sizeOrder * 0.01;
	if (slidingSpeed > dynamicSaturationSpeed) {
		dynamicFricForce = -slidingVelocity / slidingSpeed * frictionForce;
	} else {
		double effectFactor = slidingSpeed / (dynamicSaturationSpeed);
		dynamicFricForce = -slidingVelocity / slidingSpeed * frictionForce * effectFactor;
	}
	phys1.applyForce(collissionRelP1, dynamicFricForce);
	phys2.applyForce(collissionRelP2, -dynamicFricForce);

	assert(phys1.isValid());
	assert(phys2.isValid());
}

/*
	exitVector is the distance p2 must travel so that the shapes are no longer colliding
*/
void handleTerrainCollision(Part& part1, Part& part2, Position collisionPoint, Vec3 exitVector) {
	Debug::logPoint(collisionPoint, Debug::INTERSECTION);
	Physical& parent1 = *part1.parent;
	MotorizedPhysical& phys1 = *parent1.mainPhysical;

	double sizeOrder = std::min(part1.maxRadius, part2.maxRadius);
	if (lengthSquared(exitVector) <= 1E-8 * sizeOrder * sizeOrder) {
		return; // don't do anything for very small colissions
	}

	Vec3 collissionRelP1 = collisionPoint - phys1.getCenterOfMass();

	double inertia = phys1.getInertiaOfPointInDirectionRelative(collissionRelP1, exitVector);

	// Friction
	double staticFriction = part1.properties.friction * part2.properties.friction;
	double dynamicFriction = part1.properties.friction * part2.properties.friction;


	Vec3 depthForce = -exitVector * (COLLISSION_DEPTH_FORCE_MULTIPLIER * inertia);

	phys1.applyForce(collissionRelP1, depthForce);

	//Vec3 rigidBodyToPart = part1.getCFrame().getPosition() - parent1.rigidBody.getCenterOfMass();
	Vec3 partToColission = collisionPoint - part1.getPosition();
	Vec3 relativeVelocity = part1.getMotion().getVelocityOfPoint(partToColission) - part1.properties.conveyorEffect + part2.getCFrame().localToRelative(part2.properties.conveyorEffect);

	bool isImpulseColission = relativeVelocity * exitVector > 0;

	Vec3 impulse;


	double combinedBouncyness = part1.properties.bouncyness * part2.properties.bouncyness;

	if (isImpulseColission) { // moving towards the other object
		Vec3 desiredAccel = -exitVector * (relativeVelocity * exitVector) / lengthSquared(exitVector) * (1.0 + combinedBouncyness);
		Vec3 zeroRelVelImpulse = desiredAccel * inertia;
		impulse = zeroRelVelImpulse;
		phys1.applyImpulse(collissionRelP1, impulse);
		relativeVelocity += desiredAccel;
	}

	Vec3 slidingVelocity = exitVector % relativeVelocity % exitVector / lengthSquared(exitVector);

	// Compute combined inertia in the horizontal direction
	double combinedHorizontalInertia = phys1.getInertiaOfPointInDirectionRelative(collissionRelP1, slidingVelocity);

	if (isImpulseColission) {
		Vec3 maxFrictionImpulse = -exitVector % impulse % exitVector / lengthSquared(exitVector) * staticFriction;
		Vec3 stopFricImpulse = -slidingVelocity * combinedHorizontalInertia;

		Vec3 fricImpulse = (lengthSquared(stopFricImpulse) < lengthSquared(maxFrictionImpulse)) ? stopFricImpulse : maxFrictionImpulse;

		phys1.applyImpulse(collissionRelP1, fricImpulse);
	}

	double normalForce = length(depthForce);
	double frictionForce = normalForce * dynamicFriction;
	double slidingSpeed = length(slidingVelocity) + 1E-100;
	Vec3 dynamicFricForce;
	double dynamicSaturationSpeed = sizeOrder * 0.01;
	if (slidingSpeed > dynamicSaturationSpeed) {
		dynamicFricForce = -slidingVelocity / slidingSpeed * frictionForce;
	} else {
		double effectFactor = slidingSpeed / (dynamicSaturationSpeed);
		dynamicFricForce = -slidingVelocity / slidingSpeed * frictionForce * effectFactor;
	}
	phys1.applyForce(collissionRelP1, dynamicFricForce);

	assert(phys1.isValid());
}

/*
	===== World Tick =====
*/

void WorldPrototype::tick() {
	
	findColissions();

	physicsMeasure.mark(PhysicsProcess::EXTERNALS);
	applyExternalForces();

	handleColissions();

	intersectionStatistics.nextTally();
	
	handleConstraints();

	update();
}

void WorldPrototype::applyExternalForces() {
	for (ExternalForce* force : externalForces) {
		force->apply(this);
	}
}

void WorldPrototype::findColissions() {
	physicsMeasure.mark(PhysicsProcess::COLISSION_OTHER);

	curColissions.clear();

	for(const ColissionLayer& layer : layers) {
		if(layer.collidesInternally) {
			layer.getInternalColissions(curColissions);
		}
	}
	for(std::pair<int, int> collidingLayers : colissionMask) {
		getColissionsBetween(layers[collidingLayers.first], layers[collidingLayers.second], curColissions);
	}
}
void WorldPrototype::handleColissions() {
	physicsMeasure.mark(PhysicsProcess::COLISSION_HANDLING);
	for (Colission c : curColissions.freePartColissions) {
		handleCollision(*c.p1, *c.p2, c.intersection, c.exitVector);
	}
	for (Colission c : curColissions.freeTerrainColissions) {
		handleTerrainCollision(*c.p1, *c.p2, c.intersection, c.exitVector);
	}
}
void WorldPrototype::handleConstraints() {
	physicsMeasure.mark(PhysicsProcess::CONSTRAINTS);
	for (const ConstraintGroup& group : constraints) {
		group.apply();
	}
}
void WorldPrototype::update() {
	physicsMeasure.mark(PhysicsProcess::UPDATING);
	for (MotorizedPhysical* physical : iterPhysicals()) {
		physical->update(this->deltaT);
	}

	for(ColissionLayer& layer : layers) {
		layer.refresh();
	}
	age++;

	for (SoftLink* link : softLinks) {
		link->update();
	}
	
}



double WorldPrototype::getTotalKineticEnergy() const {
	double total = 0.0;
	for(const MotorizedPhysical* p : iterPhysicals()) {
		total += p->getKineticEnergy();
	}
	return total;
}
double WorldPrototype::getTotalPotentialEnergy() const {
	double total = 0.0;
	for(ExternalForce* force : externalForces) {
		total += force->getTotalPotentialEnergyForThisForce(this);
	}
	return total;
}
double WorldPrototype::getPotentialEnergyOfPhysical(const MotorizedPhysical& p) const {
	double total = 0.0;
	for (ExternalForce* force : externalForces) {
		total += force->getPotentialEnergyForObject(this, p);
	}
	return total;
}
double WorldPrototype::getTotalEnergy() const {
	return getTotalKineticEnergy() + getTotalPotentialEnergy();
}
