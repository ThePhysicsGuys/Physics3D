
#include "world.h"

#include <iostream>

#include "../util/Log.h"

#include "math\mathUtil.h"

#include "debug.h"

World::World() {

}

void World::tick(double deltaT) {

	Debug::logVec(Vec3(1.3, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Debug::INFO);
	Debug::logVec(Vec3(1.4, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Debug::FORCE);
	Debug::logVec(Vec3(1.5, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Debug::MOMENT);
	Debug::logVec(Vec3(1.6, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Debug::IMPULSE);
	Debug::logVec(Vec3(1.7, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Debug::POSITION);
	Debug::logVec(Vec3(1.8, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Debug::VELOCITY);
	Debug::logVec(Vec3(1.9, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Debug::ANGULAR_VELOCITY);

	
	Vec3* buf = (Vec3*) alloca(5000 * sizeof(Vec3));
	
	for (Physical& p : physicals) {
		double l = p.cframe.position.lengthSquared();
		p.applyForceAtCenterOfMass(Vec3(0.0, -10.0, 0.0) * p.mass);

		Shape transformed = p.part.hitbox.localToGlobal(p.cframe, buf);

		
		for (int i = 0; i < transformed.vCount; i++) {
			// logVec(Vec3(0.0, 2.0, 0.3), Vec3(1.0, 1.0, 1.0));

			Vec3 v = transformed.vertices[i];

			Vec3 velocityOfPoint = p.getVelocityOfPoint(v - p.getCenterOfMass());
			Debug::logVec(transformed.vertices[i], velocityOfPoint, Debug::VELOCITY);

			if (v.y < 0) {

				double downwardSpeed = velocityOfPoint * Vec3(0.0, 1.0, 0.0);

				if(downwardSpeed < 0)
					p.applyForce(v - p.getCenterOfMass(), Vec3(0.0, -10 * downwardSpeed * p.mass, 0.0));

				p.applyForce(v - p.getCenterOfMass(), Vec3(0, -100 * v.y * p.mass, 0));
			}

			// p.applyMoment(Vec3(1.0, 0.0, 0.0));
		}
	}

	for (Physical& p : physicals) {
		p.update(deltaT);

		Debug::logVec(p.getCenterOfMass(), p.angularVelocity, Debug::ANGULAR_VELOCITY);
		Debug::logCFrame(p.cframe, Debug::OBJECT_CFRAME);
	}
}

void World::addObject(Physical p) {
	physicals.push_back(p);
}
