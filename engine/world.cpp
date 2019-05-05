
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

#include <vector>

WorldPrototype::WorldPrototype() : PartContainer(20) {}

/*
	Compute combined inertia between to objects in a direction
*/
double computeCombinedInertiaBetween(const Physical& first, const Physical& second, const Vec3& localColissionFirst, const Vec3& localColissionSecond, const Vec3& colissionNormal) {
	SymmetricMat3 accMat1 = first.getPointAccelerationMatrix(localColissionFirst);
	SymmetricMat3 accMat2 = second.getPointAccelerationMatrix(localColissionSecond);

	SymmetricMat3 accelToForceMat = ~(accMat1 + accMat2);
	Vec3 imaginaryForceForAcceleration = accelToForceMat * colissionNormal;
	double forcePerAccelRatio = imaginaryForceForAcceleration * colissionNormal / colissionNormal.lengthSquared();

	if(forcePerAccelRatio != forcePerAccelRatio) {
		Log::error("ForcePerAccelRatio is bad! %f", forcePerAccelRatio);
		__debugbreak();
	}
	return forcePerAccelRatio;
}

/*
	exitVector is the distance p2 must travel so that the shapes are no longer colliding
*/
void handleCollision(Part& part1, Part& part2, Vec3 collisionPoint, Vec3 exitVector) {
	Physical& p1 = *part1.parent;
	Physical& p2 = *part2.parent;

	double sizeOrder = std::min(p1.part->maxRadius, p2.part->maxRadius);
	if(exitVector.lengthSquared() <= 1E-8 * sizeOrder*sizeOrder) {
		return; // don't do anything for very small colissions
	}

	Vec3 collissionRelP1 = collisionPoint - p1.getCenterOfMass();
	Vec3 collissionRelP2 = collisionPoint - p2.getCenterOfMass();

	double combinedInertia = computeCombinedInertiaBetween(p1, p2, p1.part->cframe.relativeToLocal(collissionRelP1), p2.part->cframe.relativeToLocal(collissionRelP2), exitVector);

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

	double sizeOrder = std::min(anchored.part->maxRadius, freePhys.part->maxRadius);
	if(exitVector.lengthSquared() <= 1E-20 * sizeOrder*sizeOrder) {
		return; // don't do anything for very small colissions
	}

	Vec3 collissionRelAnchored = collisionPoint - anchored.getCenterOfMass();
	Vec3 collissionRelFree = collisionPoint - freePhys.getCenterOfMass();

	double combinedInertia = freePhys.getInertiaOfPointInDirection(freePhys.part->cframe.relativeToLocal(collissionRelFree), exitVector);

	Vec3 depthForce = COLLISSION_DEPTH_FORCE_MULTIPLIER * combinedInertia * exitVector;

	freePhys.applyForce(collissionRelFree, depthForce);
	

	Vec3 relativeVelocity = anchored.getVelocityOfPoint(collissionRelAnchored) - freePhys.getVelocityOfPoint(collissionRelFree);

	Vec3 relVelNormalComponent = relativeVelocity * exitVector * exitVector / exitVector.lengthSquared();
	Vec3 relVelSidewaysComponent = -relativeVelocity % exitVector % exitVector / exitVector.lengthSquared();

	Debug::logVec(collisionPoint, relativeVelocity, Debug::VELOCITY);
	Debug::logVec(collisionPoint, relVelNormalComponent, Debug::VELOCITY);
	Debug::logVec(collisionPoint, relVelSidewaysComponent, Debug::VELOCITY);

	if(relativeVelocity * exitVector > 0) { // moving towards the other object
		Vec3 normalVelForce = -relVelNormalComponent * combinedInertia * COLLISSION_RELATIVE_VELOCITY_FORCE_MULTIPLIER;
		// anchored.applyForce(collissionRelAnchored, normalVelForce);
		freePhys.applyForce(collissionRelFree, -normalVelForce);

		Vec3 frictionForce = -(anchoredPart.properties.friction + freePart.properties.friction) / 2 * relVelSidewaysComponent * combinedInertia * COLLISSION_RELATIVE_VELOCITY_FORCE_MULTIPLIER;
		// anchored.applyForce(collissionRelAnchored, frictionForce);
		freePhys.applyForce(collissionRelFree, -frictionForce);
	}
}

struct Colission {
	Part* p1;
	Part* p2;
	Vec3 intersection;
	Vec3 exitVector;
};

// returns anchoredColissions offset
size_t findColissions(WorldPrototype& world, std::vector<Colission>& colissions) {
	for(Part& anchoredPart: world.iterAnchoredParts()) {
		for(Part& freePart: world.iterFreeParts()) {

			double maxRadiusBetween = anchoredPart.maxRadius + freePart.maxRadius;

			Vec3 deltaPosition = anchoredPart.cframe.position - freePart.cframe.position;
			double distanceSqBetween = deltaPosition.lengthSquared();

			if(distanceSqBetween > maxRadiusBetween*maxRadiusBetween) {
				intersectionStatistics.addToTally(IntersectionResult::DISTANCE_REJECT, 1);
				continue;
			}

			Vec3 intersection;
			Vec3 exitVector;
			if(anchoredPart.intersects(freePart, intersection, exitVector)) {
				intersectionStatistics.addToTally(IntersectionResult::COLISSION, 1);
				colissions.push_back(Colission{&anchoredPart, &freePart, intersection, exitVector});
			} else {
				intersectionStatistics.addToTally(IntersectionResult::GJK_REJECT, 1);
			}
			physicsMeasure.mark(PhysicsProcess::COLISSION_OTHER);
		}
	}

	size_t anchoredOffset = colissions.size();

	PartIteratorFactory iter = world.iterFreeParts();
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

			Vec3 intersection;
			Vec3 exitVector;
			if(p1.intersects(p2, intersection, exitVector)) {
				intersectionStatistics.addToTally(IntersectionResult::COLISSION, 1);
				colissions.push_back(Colission{&p1, &p2, intersection, exitVector});
			} else {
				intersectionStatistics.addToTally(IntersectionResult::GJK_REJECT, 1);
			}
			physicsMeasure.mark(PhysicsProcess::COLISSION_OTHER);
		}
	}

	return anchoredOffset;
}


void WorldPrototype::tick(double deltaT) {
	SharedLockGuard mutLock(lock);
#ifdef USE_TRANSFORMATIONS
	physicsMeasure.mark(PhysicsProcess::TRANSFORMS);
	std::shared_ptr<Vec3> vecBuf(new Vec3[getTotalVertexCount()], std::default_delete<Vec3[]>());
	Vec3* vecBufIndex = vecBuf.get();
	for(Part& part : *this) {
		const Shape& curShape = part.hitbox;
		CFrame cframe = part.cframe;
		part.transformed = curShape.localToGlobal(cframe, vecBufIndex);
		vecBufIndex += curShape.vertexCount;
	}
#endif
	physicsMeasure.mark(PhysicsProcess::EXTERNALS);
	applyExternalForces();

	physicsMeasure.mark(PhysicsProcess::COLISSION_OTHER);
	std::vector<Colission> colissions;
	size_t anchoredOffset = findColissions(*this, colissions);

	physicsMeasure.mark(PhysicsProcess::COLISSION_HANDLING);
	for(int i = 0; i < anchoredOffset; i++) {
		Colission c = colissions[i];
		handleAnchoredCollision(*c.p1, *c.p2, c.intersection, c.exitVector);
	}
	for(int i = anchoredOffset; i < colissions.size(); i++) {
		Colission c = colissions[i];
		handleCollision(*c.p1, *c.p2, c.intersection, c.exitVector);
	}

	intersectionStatistics.nextTally();

	physicsMeasure.mark(PhysicsProcess::WAIT_FOR_LOCk);
	// mutLock.upgrade();
	physicsMeasure.mark(PhysicsProcess::UPDATING);
	for(Physical& physical : iterPhysicals()) {
		physical.update(deltaT);
	}

	physicsMeasure.mark(PhysicsProcess::OTHER);
	processQueue();
}

size_t WorldPrototype::getTotalVertexCount() {
	size_t total = 0;
	for(const Part& part : *this)
		total += part.hitbox.vertexCount;
	return total;
}

void WorldPrototype::addPartUnsafe(Part* part, bool anchored) {
	add(part, anchored);
}

void WorldPrototype::processQueue() {
	std::lock_guard<std::mutex> lg(queueLock);

	while(!newPartQueue.empty()) {
		QueuedPart& qp = newPartQueue.front();
		addPartUnsafe(qp.p, qp.anchored);
		newPartQueue.pop();
	}
}

void WorldPrototype::addObject(Part* part, bool anchored) {
	if(lock.try_lock()) {
		addPartUnsafe(part, anchored);
		lock.unlock();
	} else {
		std::lock_guard<std::mutex> lg(queueLock);
		newPartQueue.push(QueuedPart{part, anchored});
	}
};

void WorldPrototype::removePart(Part* part) {
	this->remove(part);
	
}

void WorldPrototype::applyExternalForces() {}

bool WorldPrototype::isValid() const {
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
