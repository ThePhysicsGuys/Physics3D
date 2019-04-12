#include "worlds.h"

#include "../engine/debug.h"

#include "../engine/constants.h"
#include "../util/log.h"
GravityFloorWorld::GravityFloorWorld(Vec3 gravity) : gravity(gravity) {}

void GravityFloorWorld::applyExternalForces(const Shape* transformedShapes) {
	if(selectedPhysical != nullptr && !physicals.isAnchored(selectedPhysical)) {
		// Magnet force
		Vec3 absoluteSelectedPoint = selectedPhysical->part.cframe.localToGlobal(localSelectedPoint);
		Vec3 delta = magnetPoint - absoluteSelectedPoint;
		Vec3 relativeSelectedPointSpeed = selectedPhysical->getVelocityOfPoint(absoluteSelectedPoint - selectedPhysical->getCenterOfMass());
		Vec3 force = selectedPhysical->mass * delta * 20 - relativeSelectedPointSpeed;
		selectedPhysical->applyForce(absoluteSelectedPoint - selectedPhysical->getCenterOfMass(), force);
		Vec3 angular = -selectedPhysical->angularVelocity;// / (delta.length() + 1);
		selectedPhysical->applyMoment(angular);
	}
	for(int j = physicals.freePhysicalsOffset; j < physicals.physicalCount; j++) {
		Physical& physical = physicals[j];
		const Shape& transformed = transformedShapes[j];

		// Gravity force
		// physical.applyForceAtCenterOfMass((Vec3(0.0, 5.0, 0.0) - physical.getCenterOfMass() * 1.0) * physical.mass);
		physical.applyForceAtCenterOfMass(gravity * physical.mass);

		// Floor force
		for(int i = 0; i < transformed.vertexCount; i++) {
			Vec3 collisionPoint = transformed.vertices[i];
			if(collisionPoint.y >= 0) continue;

			Vec3 collissionRelP1 = collisionPoint - physical.getCenterOfMass();

			// double combinedInertia = 1 / (1 / physical.mass + 1 / p2.mass);

			Vec3 exitVector = Vec3(0, collisionPoint.y, 0);

			Vec3 depthForce = 5000 * physical.mass * exitVector;

			physical.applyForce(collissionRelP1, -depthForce);
			// p2.applyForce(collissionRelP2, depthForce);

			Vec3 relativeVelocity = physical.getVelocityOfPoint(collissionRelP1);

			Vec3 relVelNormalComponent = relativeVelocity * exitVector * exitVector / exitVector.lengthSquared();
			Vec3 relVelSidewaysComponent = -relativeVelocity % exitVector % exitVector / exitVector.lengthSquared();

			Debug::logVec(collisionPoint, relativeVelocity, Debug::VELOCITY);
			Debug::logVec(collisionPoint, relVelNormalComponent, Debug::VELOCITY);
			Debug::logVec(collisionPoint, relVelSidewaysComponent, Debug::VELOCITY);


			if(relativeVelocity * exitVector > 0) { // moving towards the other object
				Vec3 normalVelForce = -relVelNormalComponent * physical.mass * 10;
				physical.applyForce(collissionRelP1, normalVelForce);
				// p2.applyForce(collissionRelP2, -normalVelForce);

				Vec3 frictionForce = -physical.part.properties.friction * relVelSidewaysComponent * physical.mass * 10;
				physical.applyForce(collissionRelP1, frictionForce);
				// p2.applyForce(collissionRelP2, -frictionForce);
			}
		}
	}
}
