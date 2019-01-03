
#include "world.h"

#include <iostream>

#include "../util/Log.h"

#include "math\mathUtil.h"

#include "debug.h"

World::World() {}

void World::tick(double deltaT) {

	Vec3* vecBuf = (Vec3*) alloca(getTotalVertexCount() * sizeof(Vec3));
	Shape* transformedShapes = (Shape*) alloca(physicals.size() * sizeof(Shape));

	Vec3* vecBufIndex = vecBuf;

	for(int i = 0; i < physicals.size(); i++) {
		const Shape& curShape = physicals[i].part.hitbox;

		transformedShapes[i] = curShape.localToGlobal(physicals[i].cframe, vecBufIndex);
		vecBufIndex += curShape.vCount;
	}

	applyExternalForces(transformedShapes);

	for (Physical& p : physicals) {
		p.update(deltaT);

		Debug::logVec(p.getCenterOfMass(), p.angularVelocity, Debug::ANGULAR_VELOCITY);
		Debug::logCFrame(p.cframe, Debug::OBJECT_CFRAME);
	}
}

size_t World::getTotalVertexCount() {
	size_t total = 0;
	for(const Physical& p : physicals)
		total += p.part.hitbox.vCount;
	return total;
}

void World::addObject(Physical p) {
	physicals.push_back(p);
}

void World::applyExternalForces(const Shape* transformedShapes) {}

