#include "worlds.h"

#include "../engine/debug.h"

#include "../engine/constants.h"
#include "../util/log.h"
#include "application.h"
#include "input/standardInputHandler.h"
#include "view/screen.h"
#include "options/keyboardOptions.h"

#include "../engine/math/tempUnsafeCasts.h"

#define PICKER_STRENGTH 100
#define PICKER_SPEED_STRENGTH 12
#define PICKER_ANGULAR_REDUCE_STRENGTH 50

void MagnetWorld::applyExternalForces() {
	if (selectedPart != nullptr && !isAnchored(selectedPart->parent)) {
		Physical* selectedPhysical = selectedPart->parent;
		CFrame cframe = selectedPhysical->getCFrame();
		// Magnet force
		Position absoluteSelectedPoint = TEMP_CAST_CFRAME_TO_GLOBALCFRAME(selectedPart->cframe).localToGlobal(localSelectedPoint);
		Vec3 delta = magnetPoint - absoluteSelectedPoint;

		Position centerOfmass = TEMP_CAST_VEC_TO_POSITION(selectedPhysical->getCenterOfMass());

		Vec3 relativeSelectedPointSpeed = selectedPhysical->getVelocityOfPoint(absoluteSelectedPoint - centerOfmass);
		Vec3 force = selectedPhysical->mass * (delta * PICKER_STRENGTH - relativeSelectedPointSpeed * PICKER_SPEED_STRENGTH);
		selectedPhysical->applyForce(absoluteSelectedPoint - centerOfmass, force);
		Vec3 angular = -cframe.localToRelative(selectedPhysical->inertia * cframe.relativeToLocal(selectedPhysical->angularVelocity)) * PICKER_ANGULAR_REDUCE_STRENGTH;// / (delta.length() + 1);
		selectedPhysical->applyMoment(angular);
	}
}

GravityWorld::GravityWorld(Vec3 gravity) : gravity(gravity) {}

void GravityWorld::applyExternalForces() {
	MagnetWorld::applyExternalForces();
	// Gravity force
	for(Physical& physical : iterFreePhysicals()) {
		// object.applyForceAtCenterOfMass((Vec3(0.0, 5.0, 0.0) - object.getCenterOfMass() * 1.0) * object.mass);
		physical.applyForceAtCenterOfMass(gravity * physical.mass);
	}

	// Player movement
	if(!flying) {
		/*Vec3 upOrientation = player->cframe.localToRelative(Vec3(0.0, 1.0, 0.0));
		Vec3 moment = -Vec3(0.0, 1.0, 0.0) % upOrientation * 700;
		player->parent->applyMoment(moment * player->parent->mass);
		player->parent->angularVelocity *= 0.98;
		player->parent->velocity *= 0.999;*/

		Camera& camera = getCamera();
		Vec3 playerX = camera.cframe.rotation.transpose() * Vec3(1, 0, 0);
		Vec3 playerZ = camera.cframe.rotation.transpose() * Vec3(0, 0, 1);
		Vec3 newPos = TEMP_CAST_POSITION_TO_VEC(camera.cframe.position);
		Debug::logVector(newPos - playerZ, playerX, Debug::INFO_VEC);
		if(handler->anyKey) {
			Vec3 UP(0, 1, 0);
			Vec3 forward = (playerZ % UP % UP).normalize();
			Vec3 right = -(playerX % UP % UP).normalize();
			bool leftDragging = handler->leftDragging;
			if(handler->getKey(KeyboardOptions::Move::forward)) player->parent->applyForceAtCenterOfMass(forward * player->parent->mass * 20.0);
			if(handler->getKey(KeyboardOptions::Move::backward)) player->parent->applyForceAtCenterOfMass(-forward * player->parent->mass * 20.0);
			if(handler->getKey(KeyboardOptions::Move::right)) player->parent->applyForceAtCenterOfMass(right * player->parent->mass * 20.0);
			if(handler->getKey(KeyboardOptions::Move::left)) player->parent->applyForceAtCenterOfMass(-right * player->parent->mass * 20.0);
			if(handler->getKey(KeyboardOptions::Move::jump))
				player->parent->applyForceAtCenterOfMass(Vec3(0.0, 50.0 * player->parent->mass, 0.0));
		}
	}
}

double GravityWorld::getTotalPotentialEnergy() const {
	double total = 0.0;
	for(const Physical& p : iterFreePhysicals()) {
		total += -p.getCenterOfMass() * gravity * p.mass;
	}
	return total;
}
double GravityWorld::getPotentialEnergyOfPhysical(const Physical& p) const {
	return -p.getCenterOfMass() * gravity * p.mass;
}
