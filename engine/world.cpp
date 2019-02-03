
#include "world.h"

#include <iostream>
#include <cmath>

#include "../util/Log.h"

#include "math\mathUtil.h"

#include "debug.h"

World::World() {}

void handleTriangleIntersect(Physical& p1, Physical& p2, const Shape& transfI, const Shape& transfJ, Triangle t1, Triangle t2) {
	for(int k = 0; k < 3; k++) {
		Vec3 start = transfI.vertices[t1[k]];
		Vec3 end = transfI.vertices[t1[(k + 1) % 3]];
		Vec3 edgeRay = end - start;

		// Vec3 normalVec = transfJ.getNormalVecOfTriangle(t2);
		Vec3 position = transfJ.vertices[t2.firstIndex];
		Vec3 u = transfJ.vertices[t2.secondIndex] - position;
		Vec3 v = transfJ.vertices[t2.thirdIndex] - position;


		Vec3 normalVec = u % v;

		double t = (position - start) * normalVec / (edgeRay * normalVec);
		if(t > 0 && t < 1) {
			Vec3 intersection = start + t*edgeRay;

			Vec3 w = intersection - position;

			double denom = (u*v)*(u*v) - (u*u)*(v*v);
			double s = ((u*v)*(w*v) - (v*v)*(w*u)) / denom;
			double t = ((u*v)*(w*u) - (u*u)*(w*v)) / denom;

			if(s > 0 && t > 0 && s + t < 1) {
				// edgeRay intersects t2!

				Vec3 v1 = p1.getVelocityOfPoint(intersection - p1.getCenterOfMass());
				Vec3 v2 = p2.getVelocityOfPoint(intersection - p2.getCenterOfMass());
				Vec3 relativeVelocity = v2 - v1;

				Debug::logVec(intersection, relativeVelocity, Debug::VELOCITY);

				Vec3 relativeVelProjection = (relativeVelocity * normalVec) * normalVec / normalVec.lengthSquared();

				Debug::logVec(intersection, relativeVelProjection, Debug::IMPULSE);

				// Vec3 depthForceFactor = normalVec.normalize();
				Vec3 depthForceFactor = abs(edgeRay * normalVec) * normalVec / normalVec.lengthSquared();
				// Vec3 relativeSpeedFactor = (normalVec * relativeVelocity > 0) ? relativeVelProjection : Vec3();
				Vec3 force = (depthForceFactor * 50/* + relativeSpeedFactor*10*/) * 1 / (1 / p1.mass + 1 / p2.mass);
				p1.applyForce(intersection - p1.getCenterOfMass(), force);
				p2.applyForce(intersection - p2.getCenterOfMass(), -force);

				Debug::logVec(Vec3(), intersection, Debug::INFO);
			}
		}
	}
}

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

	// Compute object collisions
	
	for(int i = 0; i < physicals.size(); i++) {
		Physical& p1 = physicals[i];
		Shape transfI = transformedShapes[i];
		for(int j = i+1; j < physicals.size(); j++) {
			Physical& p2 = physicals[j];
			Shape transfJ = transformedShapes[j];
			
			for(const Triangle& t1:transfI.iterTriangles()) {
				for(const Triangle& t2 : transfJ.iterTriangles()) {
					handleTriangleIntersect(p1, p2, transfI, transfJ, t1, t2);
					handleTriangleIntersect(p2, p1, transfJ, transfI, t2, t1);
				}
			}
		}
	}

	for (int i = 0; i < physicals.size(); i++) {
		Physical& p = physicals[i];
		p.update(deltaT);

		Debug::logVec(p.getCenterOfMass(), p.angularVelocity, Debug::ANGULAR_VELOCITY);
		Debug::logVec(p.getCenterOfMass(), p.part.cframe.localToRelative(Vec3(p.inertia.m00/p.mass, 0, 0)), Debug::INFO);
		Debug::logVec(p.getCenterOfMass(), p.part.cframe.localToRelative(Vec3(0, p.inertia.m11 / p.mass, 0)), Debug::INFO);
		Debug::logVec(p.getCenterOfMass(), p.part.cframe.localToRelative(Vec3(0, 0, p.inertia.m22 / p.mass)), Debug::INFO);


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

Physical& World::addObject(Part& p) {
	Physical phys(p);
	physicals.push_back(phys);
	return phys;
};

Physical& World::addObject(Shape s, CFrame location, double density, double friction) {
	Part part(s, location, density, friction);

	Physical newPhysical(part);

	addObject(newPhysical);

	return newPhysical;
};

void World::applyExternalForces(const Shape* transformedShapes) {}

