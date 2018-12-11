
#include "world.h"

#include <iostream>

#include "../util/Log.h"

#include "math\mathUtil.h"

#include "debug.h"

World::World() {

}

void World::tick(double deltaT) {
	Vec3* buf = (Vec3*) alloca(5000 * sizeof(Vec3));
	for (Physical& p : physicals) {
		double l = p.cframe.position.lengthSquared();
		p.applyForceAtCenterOfMass(Vec3(0.0, -10.0, 0.0) * p.mass);

		Shape transformed = p.part.hitbox.localToGlobal(p.cframe, buf);



		for (int i = 0; i < transformed.vCount; i++) {
			
			Vec3 v = transformed.vertices[i];
			if (v.y < 0) {
				Vec3 velocityOfPoint = p.getVelocityOfPoint(v - p.getCenterOfMass());

				double downwardSpeed = velocityOfPoint * Vec3(0.0, 1.0, 0.0);

				// logVec(transformed.vertices[i], velocityOfPoint);

				logVec(Vec3(0.0, 2.0, 0.3), Vec3(1.0, 1.0, 1.0));

				if(downwardSpeed < 0)
					p.applyForce(v - p.getCenterOfMass(), Vec3(0.0, -10 * downwardSpeed, 0.0));

				p.applyForce(v - p.getCenterOfMass(), Vec3(0, -100 * v.y * p.mass, 0));
			}
		}
	}

	for (Physical& p : physicals) {
		p.update(deltaT);
	}
}

void World::addObject(Physical p) {
	physicals.push_back(p);
}
