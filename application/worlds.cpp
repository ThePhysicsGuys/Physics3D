#include "worlds.h"

#include "../engine/debug.h"

#include "../engine/constants.h"
#include "../util/log.h"
GravityFloorWorld::GravityFloorWorld(Vec3 gravity) : gravity(gravity) {}

void GravityFloorWorld::applyExternalForces(const Shape* transformedShapes) {
	for(int j = 0; j < physicals.size(); j++) {
		Physical& physical = physicals[j];
		const Shape& transformed = transformedShapes[j];

		// Gravity force
		physical.applyForceAtCenterOfMass((gravity/* - physical.getCenterOfMass() * 0.1*/) * physical.mass);

		// Magnet force
		if (selectedPhysical == &physical) {
			Vec3 absoluteSelectedPoint = physical.part.cframe.localToGlobal(localSelectedPoint);
			Vec3 delta = magnetPoint - absoluteSelectedPoint;
			Vec3 relativeSelectedPointSpeed = physical.getVelocityOfPoint(absoluteSelectedPoint - physical.getCenterOfMass());
			Vec3 force = physical.mass * delta * 20 - relativeSelectedPointSpeed;
			physical.applyForce(absoluteSelectedPoint - physical.getCenterOfMass(), force);
			Vec3 angular = -physical.angularVelocity;// / (delta.length() + 1);
			physical.applyMoment(angular);
		}

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
