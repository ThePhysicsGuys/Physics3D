#include "worlds.h"

#include "../engine/debug.h"

GravityFloorWorld::GravityFloorWorld(Vec3 gravity) : gravity(gravity) {}

void GravityFloorWorld::applyExternalForces(const Shape* transformedShapes) {
	for(int j = 0; j < physicals.size(); j++) {
		Physical& p = physicals[j];
		const Shape& transformed = transformedShapes[j];

		p.applyForceAtCenterOfMass(gravity * p.mass);

		for(int i = 0; i < transformed.vCount; i++) {
			Vec3 v = transformed.vertices[i];

			Vec3 velocityOfPoint = p.getVelocityOfPoint(v - p.getCenterOfMass());
			Debug::logVec(transformed.vertices[i], velocityOfPoint, Debug::VELOCITY);

			if(v.y < 0) {

				double downwardSpeed = velocityOfPoint * Vec3(0.0, 1.0, 0.0);

				if(downwardSpeed < 0)
					p.applyForce(v - p.getCenterOfMass(), gravity * (downwardSpeed * p.mass));

				p.applyForce(v - p.getCenterOfMass(), gravity * (10 * v.y * p.mass));
			}
		}
	}
}
