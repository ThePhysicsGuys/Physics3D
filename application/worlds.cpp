#include "worlds.h"

#include "../engine/debug.h"

GravityFloorWorld::GravityFloorWorld(Vec3 gravity) : gravity(gravity) {}

void GravityFloorWorld::applyExternalForces(const Shape* transformedShapes) {
	for(int j = 0; j < physicals.size(); j++) {
		Physical& physical = physicals[j];
		const Shape& transformed = transformedShapes[j];

		// Gravity force
		physical.applyForceAtCenterOfMass(gravity * physical.mass);

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
			Vec3 vertex = transformed.vertices[i];

			Vec3 velocityOfPoint = physical.getVelocityOfPoint(vertex - physical.getCenterOfMass());
			// Debug::logVec(transformed.vertices[i], velocityOfPoint, Debug::VELOCITY);

			if(vertex.y < 0) {

				double downwardSpeed = velocityOfPoint * Vec3(0.0, 1.0, 0.0);

				if (downwardSpeed < 0) {
					Vec3 relativePos = vertex - physical.getCenterOfMass();
					physical.applyForce(relativePos, gravity * (downwardSpeed * physical.mass * 1.0));

					Debug::logVec(relativePos + physical.getCenterOfMass(), gravity * (downwardSpeed * physical.mass), Debug::INFO);
				}

				physical.applyForce(vertex - physical.getCenterOfMass(), gravity * (10 * vertex.y * physical.mass));
			}
		}
	}
}
