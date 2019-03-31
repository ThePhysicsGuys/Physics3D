
#include "world.h"

#include <iostream>
#include <cmath>

#include "../util/Log.h"

#include "math\mathUtil.h"

#include "debug.h"

#include "constants.h"

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

/*
	exitVector is the distance p2 must travel so that the shapes are no longer colliding
*/
void handleCollision(Physical& p1, Physical& p2, Vec3 collisionPoint, Vec3 exitVector) {

	Vec3 collissionRelP1 = collisionPoint - p1.getCenterOfMass();
	Vec3 collissionRelP2 = collisionPoint - p2.getCenterOfMass();

	double combinedInertia = 1 / (1 / p1.mass + 1 / p2.mass);

	Vec3 depthForce = COLLISSION_DEPTH_FORCE_MULTIPLIER * combinedInertia * exitVector;

	p1.applyForce(collissionRelP1, -depthForce);
	p2.applyForce(collissionRelP2, depthForce);


	Vec3 relativeVelocity = p1.getVelocityOfPoint(collissionRelP1) - p2.getVelocityOfPoint(collissionRelP2);

	Vec3 relVelNormalComponent = relativeVelocity * exitVector * exitVector / exitVector.lengthSquared();
	Vec3 relVelSidewaysComponent = -relativeVelocity % exitVector % exitVector / exitVector.lengthSquared();

	Debug::logVec(collisionPoint, relativeVelocity, Debug::VELOCITY);
	Debug::logVec(collisionPoint, relVelNormalComponent, Debug::VELOCITY);
	Debug::logVec(collisionPoint, relVelSidewaysComponent, Debug::VELOCITY);

	

	if(relativeVelocity * exitVector > 0) { // moving towards the other object
		Vec3 normalVelForce = -relVelNormalComponent * combinedInertia * COLLISSION_RELATIVE_VELOCITY_FORCE_MULTIPLIER;
		p1.applyForce(collissionRelP1, normalVelForce);
		p2.applyForce(collissionRelP2, -normalVelForce);

		Vec3 frictionForce = -(p1.part.properties.friction + p2.part.properties.friction)/2 * relVelSidewaysComponent * combinedInertia * COLLISSION_RELATIVE_VELOCITY_FORCE_MULTIPLIER;
		p1.applyForce(collissionRelP1, frictionForce);
		p2.applyForce(collissionRelP2, -frictionForce);
	}
}

void World::tick(double deltaT) {
	lock.lock();

	Vec3* vecBuf = (Vec3*) alloca(getTotalVertexCount() * sizeof(Vec3));
	Shape* transformedShapes = new Shape[physicals.size()];

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
			
			double maxRadiusBetween = p1.circumscribedSphere.radius + p2.circumscribedSphere.radius;

			Vec3 globalCenterOfPos1 = p1.part.cframe.localToGlobal(p1.circumscribedSphere.origin);
			Vec3 globalCenterOfPos2 = p2.part.cframe.localToGlobal(p2.circumscribedSphere.origin);
			double distanceSqBetween = (globalCenterOfPos1 - globalCenterOfPos2).lengthSquared();

			if(distanceSqBetween > maxRadiusBetween*maxRadiusBetween) {
				continue;
			}

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

	delete[] transformedShapes;

	lock.unlock();
}

size_t World::getTotalVertexCount() {
	size_t total = 0;
	for(const Physical& physical : physicals)
		total += physical.part.hitbox.vertexCount;
	return total;
}

void World::addObject(Physical& part) {
	lock.lock();
	physicals.push_back(part);
	lock.unlock();
}

Physical& World::addObject(Part& part) {
	lock.lock();
	Physical physical(part);
	physicals.push_back(physical);

	lock.unlock();
	return physical;
};

Physical& World::addObject(Shape shape, CFrame location, double density, double friction) {
	Part part(shape, location, density, friction);
	Physical newPhysical(part);
	addObject(newPhysical);

	return newPhysical;
};

void World::applyExternalForces(const Shape* transformedShapes) {}

