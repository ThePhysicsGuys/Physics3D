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
		/*if (selectedPhysical == &physical) {
			Vec3 absoluteAttachPoint = physical.cframe.localToGlobal(relativeAttachPoint);
			Vec3 delta = magnetTarget - absoluteAttachPoint;
			physical.applyForce(absoluteAttachPoint - physical.getCenterOfMass(), delta * 0.01);
		}*/

		// Floor force
		for(int i = 0; i < transformed.vCount; i++) {
			Vec3 vertex = transformed.vertices[i];

			Vec3 velocityOfPoint = physical.getVelocityOfPoint(vertex - physical.getCenterOfMass());
			Debug::logVec(transformed.vertices[i], velocityOfPoint, Debug::VELOCITY);

			if(vertex.y < 0) {

				double downwardSpeed = velocityOfPoint * Vec3(0.0, 1.0, 0.0);

				if(downwardSpeed < 0)
					physical.applyForce(vertex - physical.getCenterOfMass(), gravity * (downwardSpeed * physical.mass));

				physical.applyForce(vertex - physical.getCenterOfMass(), gravity * (10 * vertex.y * physical.mass));
			}
		}
	}
}
