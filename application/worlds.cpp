#include "core.h"

#include "worlds.h"

#include "../physics/debug.h"
#include "../physics/constants.h"
#include "application.h"
#include "input/standardInputHandler.h"
#include "view/screen.h"
#include "../engine/options/keyboardOptions.h"

#define PICKER_STRENGTH 100
#define PICKER_SPEED_STRENGTH 12
#define PICKER_ANGULAR_REDUCE_STRENGTH 50
#define RUN_SPEED 5
#define JUMP_SPEED 6
#define AIR_RUN_SPEED_FACTOR 2

PlayerWorld::PlayerWorld(double deltaT) : SynchronizedWorld<ExtendedPart>(deltaT) {}

void PlayerWorld::applyExternalForces() {
	SynchronizedWorld<ExtendedPart>::applyExternalForces();

	if (selectedPart != nullptr && !selectedPart->parent->anchored) {
		MotorizedPhysical* selectedPhysical = selectedPart->parent->mainPhysical;
		GlobalCFrame cframe = selectedPhysical->getCFrame();
		// Magnet force
		Position absoluteSelectedPoint = selectedPart->getCFrame().localToGlobal(localSelectedPoint);
		Vec3 delta = magnetPoint - absoluteSelectedPoint;

		Position centerOfmass = selectedPhysical->getCenterOfMass();

		Vec3 relativeSelectedPointSpeed = selectedPhysical->getMotion().getVelocityOfPoint(absoluteSelectedPoint - centerOfmass);
		Vec3 force = selectedPhysical->totalMass * (delta * PICKER_STRENGTH - relativeSelectedPointSpeed * PICKER_SPEED_STRENGTH);
		selectedPhysical->applyForceToPhysical(absoluteSelectedPoint - centerOfmass, force);
		
		selectedPhysical->motionOfCenterOfMass.angularVelocity *= 0.8;
	}

	// Player movement
	if (!screen.camera.flying) {
		ExtendedPart* player = screen.camera.attachment;

		Vec3f playerX = screen.camera.cframe.rotation * Vec3(1, 0, 0);
		Vec3f playerZ = screen.camera.cframe.rotation * Vec3(0, 0, 1);
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

		Vec3 desiredSpeed = runVector;
		Vec3 actualSpeed = player->parent->getMotion().velocity;

		Vec3 speedToGain = desiredSpeed - actualSpeed;

		speedToGain.y = 0;

		player->parent->mainPhysical->applyForceAtCenterOfMass(speedToGain * player->getMass() * AIR_RUN_SPEED_FACTOR);

		if (handler->getKey(KeyboardOptions::Move::jump))
			runVector += Vec3(0, JUMP_SPEED, 0);

		player->properties.conveyorEffect = runVector;
	}
}
