#include "worlds.h"

#include "../engine/debug.h"

#include "../engine/constants.h"
#include "../util/log.h"
#include "application.h"
#include "standardInputHandler.h"
#include "view/screen.h"

#define PICKER_STRENGTH 50
#define PICKER_SPEED_STRENGTH 10

GravityWorld::GravityWorld(Vec3 gravity) : gravity(gravity) {}

void GravityWorld::applyExternalForces() {
	if(selectedPart != nullptr && !isAnchored(selectedPart->parent)) {
		Physical* selectedPhysical = selectedPart->parent;
		// Magnet force
		Vec3 absoluteSelectedPoint = selectedPart->cframe.localToGlobal(localSelectedPoint);
		Vec3 delta = magnetPoint - absoluteSelectedPoint;
		Vec3 relativeSelectedPointSpeed = selectedPhysical->getVelocityOfPoint(absoluteSelectedPoint - selectedPhysical->getCenterOfMass());
		Vec3 force = selectedPhysical->mass * delta * PICKER_STRENGTH - relativeSelectedPointSpeed * PICKER_SPEED_STRENGTH;
		selectedPhysical->applyForce(absoluteSelectedPoint - selectedPhysical->getCenterOfMass(), force);
		Vec3 angular = -selectedPhysical->angularVelocity;// / (delta.length() + 1);
		selectedPhysical->applyMoment(angular);
	}
	// Gravity force
	for(Physical& physical : iterFreePhysicals()) {
		// physical.applyForceAtCenterOfMass((Vec3(0.0, 5.0, 0.0) - physical.getCenterOfMass() * 1.0) * physical.mass);
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
		Vec3f playerX = camera.cframe.rotation.transpose() * Vec3f(1, 0, 0);
		Vec3f playerZ = camera.cframe.rotation.transpose() * Vec3f(0, 0, 1);
		Debug::logVec(camera.cframe.position - playerZ, playerX, Debug::INFO_VEC);
		if(handler->anyKey) {
			Vec3 UP(0, 1, 0);
			Vec3 forward = (playerZ % UP % UP).normalize();
			Vec3 right = -(playerX % UP % UP).normalize();
			bool leftDragging = handler->leftDragging;
			if(handler->getKey(GLFW_KEY_W)) player->parent->applyForceAtCenterOfMass(forward * player->parent->mass * 20.0);
			if(handler->getKey(GLFW_KEY_S)) player->parent->applyForceAtCenterOfMass(-forward * player->parent->mass * 20.0);
			if(handler->getKey(GLFW_KEY_D)) player->parent->applyForceAtCenterOfMass(right * player->parent->mass * 20.0);
			if(handler->getKey(GLFW_KEY_A)) player->parent->applyForceAtCenterOfMass(-right * player->parent->mass * 20.0);
			if(handler->getKey(GLFW_KEY_SPACE))
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
