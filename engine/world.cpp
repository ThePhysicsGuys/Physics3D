
#include "world.h"

#include <iostream>

#include "../util/Log.h"

World::World() {

}

void World::tick(double deltaT) {
	for (Physical& p : physicals) {
		double l = p.cframe.position.lengthSquared();
		p.applyForce(Vec3(), p.cframe.position * (-10.0 / ((l > 0.01)? l:1.0)));
	}
	for (Physical& p : physicals) {
		p.update(deltaT);
	}
}

void World::addObject(Physical p) {
	physicals.push_back(p);
}
