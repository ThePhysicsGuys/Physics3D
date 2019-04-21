
#include "world.h"

#include <iostream>
#include <cmath>
#include <algorithm>

#include "../util/Log.h"

#include "math/mathUtil.h"

#include "debug.h"
#include "constants.h"
#include "sharedLockGuard.h"
#include "physicsProfiler.h"
#include "engineException.h"



World::World() : PartContainer(20) {}


// typedef Part AnchoredPart;

/*
	exitVector is the distance p2 must travel so that the shapes are no longer colliding
*/
void handleCollision(Part& part1, Part& part2, Vec3 collisionPoint, Vec3 exitVector) {
	Physical& p1 = *part1.parent;
	Physical& p2 = *part2.parent;

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

		Vec3 frictionForce = -(part1.properties.friction + part2.properties.friction)/2 * relVelSidewaysComponent * combinedInertia * COLLISSION_RELATIVE_VELOCITY_FORCE_MULTIPLIER;
		p1.applyForce(collissionRelP1, frictionForce);
		p2.applyForce(collissionRelP2, -frictionForce);
	}
}

/*
exitVector is the distance the free physical must travel so that the shapes are no longer colliding
*/
void handleAnchoredCollision(Part& anchoredPart, Part& freePart, Vec3 collisionPoint, Vec3 exitVector) {
	Physical& anchored = *anchoredPart.parent;
	Physical& freePhys = *freePart.parent;

	Vec3 collissionRelP1 = collisionPoint - anchored.getCenterOfMass();
	Vec3 collissionRelP2 = collisionPoint - freePhys.getCenterOfMass();

	double combinedInertia = freePhys.mass;

	Vec3 depthForce = COLLISSION_DEPTH_FORCE_MULTIPLIER * combinedInertia * exitVector;

	// anchored.applyForce(collissionRelP1, -depthForce);
	freePhys.applyForce(collissionRelP2, depthForce);


	Vec3 relativeVelocity = anchored.getVelocityOfPoint(collissionRelP1) - freePhys.getVelocityOfPoint(collissionRelP2);

	Vec3 relVelNormalComponent = relativeVelocity * exitVector * exitVector / exitVector.lengthSquared();
	Vec3 relVelSidewaysComponent = -relativeVelocity % exitVector % exitVector / exitVector.lengthSquared();

	Debug::logVec(collisionPoint, relativeVelocity, Debug::VELOCITY);
	Debug::logVec(collisionPoint, relVelNormalComponent, Debug::VELOCITY);
	Debug::logVec(collisionPoint, relVelSidewaysComponent, Debug::VELOCITY);

	if(relativeVelocity * exitVector > 0) { // moving towards the other object
		Vec3 normalVelForce = -relVelNormalComponent * combinedInertia * COLLISSION_RELATIVE_VELOCITY_FORCE_MULTIPLIER;
		// anchored.applyForce(collissionRelP1, normalVelForce);
		freePhys.applyForce(collissionRelP2, -normalVelForce);

		Vec3 frictionForce = -(anchoredPart.properties.friction + freePart.properties.friction) / 2 * relVelSidewaysComponent * combinedInertia * COLLISSION_RELATIVE_VELOCITY_FORCE_MULTIPLIER;
		// anchored.applyForce(collissionRelP1, frictionForce);
		freePhys.applyForce(collissionRelP2, -frictionForce);
	}
}

void World::processColissions() {
	physicsMeasure.mark(PhysicsProcess::COLISSION_OTHER);

	for(Part& anchoredPart:iterAnchoredParts()) {
		const Shape& transfAnchored = anchoredPart.transformed;
		for(Part& freePart:iterFreeParts()) {

			double maxRadiusBetween = anchoredPart.maxRadius + freePart.maxRadius;

			Vec3 deltaPosition = anchoredPart.cframe.position - freePart.cframe.position;
			double distanceSqBetween = deltaPosition.lengthSquared();

			if(distanceSqBetween > maxRadiusBetween*maxRadiusBetween) {
				intersectionStatistics.addToTally(IntersectionResult::DISTANCE_REJECT, 1);
				continue;
			}

			const Shape& transfFree = freePart.transformed;

			Vec3 intersection;
			Vec3 exitVector;
			if(transfAnchored.intersects(transfFree, intersection, exitVector, deltaPosition)) {
				physicsMeasure.mark(PhysicsProcess::COLISSION_HANDLING);
				intersectionStatistics.addToTally(IntersectionResult::COLISSION, 1);
				handleAnchoredCollision(anchoredPart, freePart, intersection, exitVector);
			} else {
				physicsMeasure.mark(PhysicsProcess::COLISSION_OTHER);
				intersectionStatistics.addToTally(IntersectionResult::GJK_REJECT, 1);
			}
		}
	}

	PartIteratorFactory iter = iterFreeParts();
	PartIterator finish = iter.end();
	for(PartIterator mainIter = iter.begin(); mainIter != finish; ++mainIter) {
		Part& p1 = *mainIter;
		for(PartIterator secondIter = mainIter; ++secondIter != finish; ) {
			Part& p2 = *secondIter;

			double maxRadiusBetween = p1.maxRadius + p2.maxRadius;

			Vec3 deltaPosition = p1.cframe.position - p2.cframe.position;
			double distanceSqBetween = deltaPosition.lengthSquared();

			if(distanceSqBetween > maxRadiusBetween*maxRadiusBetween) {
				intersectionStatistics.addToTally(IntersectionResult::DISTANCE_REJECT, 1);
				continue;
			}

			const Shape& transfI = p1.transformed;
			const Shape& transfJ = p2.transformed;

			Vec3 intersection;
			Vec3 exitVector;
			if(transfI.intersects(transfJ, intersection, exitVector, deltaPosition)) {
				physicsMeasure.mark(PhysicsProcess::COLISSION_HANDLING);
				intersectionStatistics.addToTally(IntersectionResult::COLISSION, 1);
				handleCollision(p1, p2, intersection, exitVector);
			} else {
				physicsMeasure.mark(PhysicsProcess::COLISSION_OTHER);
				intersectionStatistics.addToTally(IntersectionResult::GJK_REJECT, 1);
			}
		}
	}
}

void World::tick(double deltaT) {
	SharedLockGuard mutLock(lock);

	Vec3* vecBuf = new Vec3[getTotalVertexCount()];

	Vec3* vecBufIndex = vecBuf;

	physicsMeasure.mark(PhysicsProcess::TRANSFORMS);
	for(Part& part : *this) {
		const Shape& curShape = part.hitbox;
		CFrame cframe = part.cframe;
		part.transformed = curShape.localToGlobal(cframe, vecBufIndex);
		vecBufIndex += curShape.vertexCount;
	}

	physicsMeasure.mark(PhysicsProcess::EXTERNALS);
	applyExternalForces();

	processColissions();

	intersectionStatistics.nextTally();

	physicsMeasure.mark(PhysicsProcess::UPDATING);
	mutLock.upgrade();
	for(Physical& physical : iterPhysicals()) {
		physical.update(deltaT);
	}

	physicsMeasure.mark(PhysicsProcess::OTHER);
	processQueue();

	delete[] vecBuf;
}

size_t World::getTotalVertexCount() {
	size_t total = 0;
	for(const Part& part : *this)
		total += part.hitbox.vertexCount;
	return total;
}

void World::addPartUnsafe(Part* part, bool anchored) {
	add(part, anchored);
}

void World::processQueue() {
	std::lock_guard<std::mutex> lg(queueLock);

	while(!newPartQueue.empty()) {
		QueuedPart& qp = newPartQueue.front();
		addPartUnsafe(qp.p, qp.anchored);
		newPartQueue.pop();
	}
}

void World::addObject(Part* part, bool anchored) {
	if(lock.try_lock()) {
		addPartUnsafe(part, anchored);
		lock.unlock();
	} else {
		std::lock_guard<std::mutex> lg(queueLock);
		newPartQueue.push(QueuedPart{part, anchored});
	}
};

void World::removePart(Part* part) {
	this->remove(part);
	
}

void World::applyExternalForces() {}

bool World::isValid() const {
	for(size_t i = 0; i < partCount; i++) {
		if(parts[i]->partIndex != i) {
			Log::error("part's partIndex is not it's partIndex");
			__debugbreak();
		}
	}

	for(const Part& part : *this) {
		if(part.parent->part != &part) {
			Log::error("part's parent's child is not part");
			__debugbreak();
		}
		if(!(part.parent >= physicals && part.parent < physicals + physicalCount)) {
			Log::error("part with parent not in our physicals");
			__debugbreak();
		}
		bool partIsAnchored = isAnchored(parts + part.partIndex);
		bool physicalIsAnchored = isAnchored(part.parent);
		if(partIsAnchored != physicalIsAnchored) {
			Log::error("%s part with %s parent", partIsAnchored ? "anchored":"unanchored", physicalIsAnchored ? "anchored" : "unanchored");
			__debugbreak();
		}
	}
	return true;
}
