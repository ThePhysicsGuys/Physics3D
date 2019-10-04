#include "core.h"

#include "worlds.h"

#include "../engine/debug.h"

#include "../engine/constants.h"
#include "application.h"
#include "input/standardInputHandler.h"
#include "view/screen.h"
#include "options/keyboardOptions.h"

#define PICKER_STRENGTH 100
#define PICKER_SPEED_STRENGTH 12
#define PICKER_ANGULAR_REDUCE_STRENGTH 50
#define RUN_SPEED 5

void MagnetWorld::applyExternalForces() {
	if (selectedPart != nullptr && !selectedPart->parent->anchored) {
		Physical* selectedPhysical = selectedPart->parent;
		GlobalCFrame cframe = selectedPhysical->getCFrame();
		// Magnet force
		Position absoluteSelectedPoint = selectedPart->getCFrame().localToGlobal(localSelectedPoint);
		Vec3 delta = magnetPoint - absoluteSelectedPoint;

		Position centerOfmass = selectedPhysical->getCenterOfMass();

		Vec3 relativeSelectedPointSpeed = selectedPhysical->getVelocityOfPoint(absoluteSelectedPoint - centerOfmass);
		Vec3 force = selectedPhysical->mass * (delta * PICKER_STRENGTH - relativeSelectedPointSpeed * PICKER_SPEED_STRENGTH);
		selectedPhysical->applyForce(absoluteSelectedPoint - centerOfmass, force);
		Vec3 angular = -cframe.localToRelative(selectedPhysical->inertia * cframe.relativeToLocal(selectedPhysical->angularVelocity)) * PICKER_ANGULAR_REDUCE_STRENGTH;// / (delta.length() + 1);
		selectedPhysical->applyMoment(angular);
	}
}

MagnetWorld::MagnetWorld(double deltaT) : SynchronizedWorld<ExtendedPart>(deltaT) {}
GravityWorld::GravityWorld(double deltaT, Vec3 gravity) : MagnetWorld(deltaT), gravity(gravity) {}

void GravityWorld::applyExternalForces() {
	MagnetWorld::applyExternalForces();
	// Gravity force
	for(Physical& physical : iterPhysicals()) {
		// object.applyForceAtCenterOfMass((Vec3(0.0, 5.0, 0.0) - object.getCenterOfMass() * 1.0) * object.mass);
		if(!physical.anchored) physical.applyForceAtCenterOfMass(gravity * physical.mass);
	}

	// Player movement
	if(!screen.camera.flying) {
		ExtendedPart* player = screen.camera.attachment;
		/*Vec3 upOrientation = player->cframe.localToRelative(Vec3(0.0, 1.0, 0.0));
		Vec3 moment = -Vec3(0.0, 1.0, 0.0) % upOrientation * 700;
		player->parent->applyMoment(moment * player->parent->mass);
		player->parent->angularVelocity *= 0.98;
		player->parent->velocity *= 0.999;*/

		Vec3f playerX = screen.camera.cframe.rotation.transpose() * Vec3(1, 0, 0);
		Vec3f playerZ = screen.camera.cframe.rotation.transpose() * Vec3(0, 0, 1);
		Debug::logVector(screen.camera.cframe.position - playerZ, playerX, Debug::INFO_VEC);

		Vec3 UP(0, 1, 0);
		Vec3 forward = normalize(playerZ % UP % UP);
		Vec3 right = -normalize(playerX % UP % UP);

		Vec3 total(0, 0, 0);
		if (handler->getKey(KeyboardOptions::Move::forward)) total += forward;
		if (handler->getKey(KeyboardOptions::Move::backward)) total -= forward;
		if (handler->getKey(KeyboardOptions::Move::right)) total += right;
		if (handler->getKey(KeyboardOptions::Move::left)) total -= right;

		Vec3 runVector = (lengthSquared(total) >= 0.00005) ? normalize(total) * RUN_SPEED : Vec3(0, 0, 0);

		player->conveyorEffect = runVector;



		/*if(handler->getKey(KeyboardOptions::Move::forward)) player->parent->applyForceAtCenterOfMass(forward * player->parent->mass * 20.0);
		if(handler->getKey(KeyboardOptions::Move::backward)) player->parent->applyForceAtCenterOfMass(-forward * player->parent->mass * 20.0);
		if(handler->getKey(KeyboardOptions::Move::right)) player->parent->applyForceAtCenterOfMass(right * player->parent->mass * 20.0);
		if(handler->getKey(KeyboardOptions::Move::left)) player->parent->applyForceAtCenterOfMass(-right * player->parent->mass * 20.0);*/
		if(handler->getKey(KeyboardOptions::Move::jump))
			player->parent->applyForceAtCenterOfMass(Vec3(0.0, 50.0 * player->parent->mass, 0.0));
	}
}

double GravityWorld::getTotalPotentialEnergy() const {
	double total = 0.0;
	for(const Physical& p : iterPhysicals()) {
		if (p.anchored) continue;
		total += Vec3(Position() - p.getCenterOfMass()) * gravity * p.mass;
	}
	return total;
}
double GravityWorld::getPotentialEnergyOfPhysical(const Physical& p) const {
	return Vec3(Position() - p.getCenterOfMass()) * gravity * p.mass;
}
