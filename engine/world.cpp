
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

void handleCollision(Physical& p1, Physical& p2, Vec3 collisionPoint, Vec3 exitVector) {

	double multiplier = 1 / (1 / p1.mass + 1 / p2.mass);

	Vec3 depthForce = 100 * multiplier * exitVector;

	p1.applyForce(collisionPoint - p1.getCenterOfMass(), -depthForce);
	p2.applyForce(collisionPoint - p2.getCenterOfMass(), depthForce);

	/*Vec3 v1 = p1.getVelocityOfPoint(collisionPoint - p1.getCenterOfMass());
	Vec3 v2 = p2.getVelocityOfPoint(collisionPoint - p2.getCenterOfMass());
	Vec3 relativeVelocity = v2 - v1;

	Debug::logVec(collisionPoint, relativeVelocity, Debug::VELOCITY);

	Vec3 relativeVelProjection = (relativeVelocity * exitVector) * exitVector / exitVector.lengthSquared();

	Debug::logVec(collisionPoint, relativeVelProjection, Debug::IMPULSE);

	// Vec3 depthForceFactor = normalVec.normalize();
	Vec3 depthForceFactor = exitVector;
	// Vec3 relativeSpeedFactor = (normalVec * relativeVelocity > 0) ? relativeVelProjection : Vec3();
	Vec3 force = (depthForceFactor * 50/* + relativeSpeedFactor*10*///) * 1 / (1 / p1.mass + 1 / p2.mass);
	/*p1.applyForce(collisionPoint - p1.getCenterOfMass(), force);
	p2.applyForce(collisionPoint - p2.getCenterOfMass(), -force);

	Debug::logVec(Vec3(), collisionPoint, Debug::INFO);*/
}

void World::tick(double deltaT) {

	Vec3* vecBuf = (Vec3*) alloca(getTotalVertexCount() * sizeof(Vec3));
	Shape* transformedShapes = (Shape*) alloca(physicals.size() * sizeof(Shape));

	Vec3* vecBufIndex = vecBuf;

	for(int i = 0; i < physicals.size(); i++) {
		const Shape& curShape = physicals[i].part.hitbox;

		transformedShapes[i] = curShape.localToGlobal(physicals[i].part.cframe, vecBufIndex);
		vecBufIndex += curShape.vertexCount;
	}

	applyExternalForces(transformedShapes);

	// Compute object collisions
	
	for(int i = 0; i < physicals.size(); i++) {
		Physical& p1 = physicals[i];
		Shape transfI = transformedShapes[i];
		for(int j = i+1; j < physicals.size(); j++) {
			Physical& p2 = physicals[j];
			Shape transfJ = transformedShapes[j];
			
			Vec3 intersection;
			Vec3 exitVector;
			if(transfI.intersects(transfJ, intersection, exitVector)) {
				Debug::logVec(intersection, exitVector, Debug::POSITION);
				Debug::logVec(p1.getCenterOfMass(), intersection - p1.getCenterOfMass(), Debug::INFO);
				Debug::logVec(p2.getCenterOfMass(), intersection - p2.getCenterOfMass(), Debug::INFO);
				Debug::logVec(intersection, intersection - p2.getCenterOfMass(), Debug::POSITION);

				handleCollision(p1, p2, intersection, exitVector);
			}

			/*for(const Triangle& t1:transfI.iterTriangles()) {
				for(const Triangle& t2 : transfJ.iterTriangles()) {
					handleTriangleIntersect(p1, p2, transfI, transfJ, t1, t2);
					handleTriangleIntersect(p2, p1, transfJ, transfI, t2, t1);
				}
			}*/
		}
	}

	for (int i = 0; i < physicals.size(); i++) {
		Physical& physical = physicals[i];
		physical.update(deltaT);

		/*Debug::logVec(physical.getCenterOfMass(), physical.angularVelocity, Debug::ANGULAR_VELOCITY);
		Debug::logVec(physical.getCenterOfMass(), physical.part.cframe.localToRelative(Vec3(physical.inertia.m00 / physical.mass, 0, 0)), Debug::INFO);
		Debug::logVec(physical.getCenterOfMass(), physical.part.cframe.localToRelative(Vec3(0, physical.inertia.m11 / physical.mass, 0)), Debug::INFO);
		Debug::logVec(physical.getCenterOfMass(), physical.part.cframe.localToRelative(Vec3(0, 0, physical.inertia.m22 / physical.mass)), Debug::INFO);*/

		
	}
}

size_t World::getTotalVertexCount() {
	size_t total = 0;
	for(const Physical& physical : physicals)
		total += physical.part.hitbox.vertexCount;
	return total;
}

void World::addObject(Physical& part) {
	physicals.push_back(part);
}

Physical& World::addObject(Part& part) {
	Physical physical(part);
	physicals.push_back(physical);
	return physical;
};

Physical& World::addObject(Shape shape, CFrame location, double density, double friction) {
	Part part(shape, location, density, friction);
	Physical newPhysical(part);
	addObject(newPhysical);

	return newPhysical;
};

void World::applyExternalForces(const Shape* transformedShapes) {}

