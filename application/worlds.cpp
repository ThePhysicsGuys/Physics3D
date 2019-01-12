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
			//Vec3 absoluteAttachPoint = physical.cframe.localToGlobal(relativeSelectedPoint);
			Vec3 delta = magnetPoint - absoluteSelectedPoint;
			physical.applyForce(absoluteSelectedPoint - physical.getCenterOfMass(), delta * 1000);
		}

		// Floor force
		for(int i = 0; i < transformed.vCount; i++) {
			Vec3 vertex = transformed.vertices[i];

			Vec3 velocityOfPoint = physical.getVelocityOfPoint(vertex - physical.getCenterOfMass());
			Debug::logVec(transformed.vertices[i], velocityOfPoint, Debug::VELOCITY);

			if(vertex.y < 0) {

				double downwardSpeed = velocityOfPoint * Vec3(0.0, 1.0, 0.0);

				if (downwardSpeed < 0) {
					Vec3 relativePos = vertex - physical.getCenterOfMass();
					physical.applyForce(relativePos, gravity * (downwardSpeed * physical.mass * 0.001));

					Debug::logVec(relativePos + physical.getCenterOfMass(), gravity * (downwardSpeed * physical.mass), Debug::INFO);
				}

				physical.applyForce(vertex - physical.getCenterOfMass(), gravity * (10 * vertex.y * physical.mass));
			}
		}
	}
}
