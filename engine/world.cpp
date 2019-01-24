
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

		transformedShapes[i] = curShape.localToGlobal(physicals[i].part.cframe, vecBufIndex);
		vecBufIndex += curShape.vCount;
	}

	applyExternalForces(transformedShapes);

	for (int i = 0; i < physicals.size(); i++) {
		Physical& p = physicals[i];
		p.update(deltaT);

		Debug::logVec(p.getCenterOfMass(), p.angularVelocity, Debug::ANGULAR_VELOCITY);
		Debug::logCFrame(p.part.cframe, Debug::OBJECT_CFRAME);
		// Debug::logCFrame(transformedShapes[i].getInertialEigenVectors(), Debug::INERTIAL_CFRAME);
	}
}

size_t World::getTotalVertexCount() {
	size_t total = 0;
	for(const Physical& p : physicals)
		total += p.part.hitbox.vCount;
	return total;
}

void World::addObject(Physical& p) {
	physicals.push_back(p);
}

Physical& World::addObject(Shape s, CFrame location, double density, double friction) {
	Part part(s, location, density, friction);

	Physical newPhysical(part);

	addObject(newPhysical);

	return newPhysical;
};

void World::applyExternalForces(const Shape* transformedShapes) {}

