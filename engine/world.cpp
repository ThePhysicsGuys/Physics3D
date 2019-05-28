
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

#define ELASTICITY 0.3

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
void handleCollision(Part& part1, Part& part2, Vec3 collisionPoint, Vec3 exitVector, bool anchoredColission) {
	Debug::logPoint(collisionPoint, Debug::INTERSECTION);
	Physical& p1 = *part1.parent;
	Physical& p2 = *part2.parent;

	double sizeOrder = std::min(part1.maxRadius, part2.maxRadius);
	if(exitVector.lengthSquared() <= 1E-8 * sizeOrder*sizeOrder) {
		return; // don't do anything for very small colissions
	}

	Vec3 collissionRelP1 = collisionPoint - p1.getCenterOfMass();
	Vec3 collissionRelP2 = collisionPoint - p2.getCenterOfMass();

	double combinedInertia;
	if(anchoredColission)
		combinedInertia = p2.getInertiaOfPointInDirection(p2.getCFrame().relativeToLocal(collissionRelP2), p2.getCFrame().relativeToLocal(exitVector));
	else {
		double inertia1 = p1.getInertiaOfPointInDirection(p1.getCFrame().relativeToLocal(collissionRelP1), p1.getCFrame().relativeToLocal(exitVector));
		double inertia2 = p2.getInertiaOfPointInDirection(p2.getCFrame().relativeToLocal(collissionRelP2), p2.getCFrame().relativeToLocal(exitVector));
		combinedInertia = 1 / (1 / inertia1 + 1 / inertia2);
	}
	
	Vec3 depthForce = exitVector * (COLLISSION_DEPTH_FORCE_MULTIPLIER * combinedInertia);

	if(!anchoredColission) p1.applyForce(collissionRelP1, -depthForce);
	p2.applyForce(collissionRelP2, depthForce);


	Vec3 relativeVelocity = p1.getVelocityOfPoint(collissionRelP1) - p2.getVelocityOfPoint(collissionRelP2);

	if(relativeVelocity * exitVector > 0) { // moving towards the other object
		Vec3 desiredAccel = -exitVector * (relativeVelocity * exitVector) / exitVector.lengthSquared();
		Vec3 zeroRelVelImpulse = desiredAccel * combinedInertia;
		Vec3 impulse = zeroRelVelImpulse * (1.0+ELASTICITY);
		if(!anchoredColission) p1.applyImpulse(collissionRelP1, impulse);
		p2.applyImpulse(collissionRelP2, -impulse);
		Vec3 relativeVelocityAfter = p1.getVelocityOfPoint(collissionRelP1) - p2.getVelocityOfPoint(collissionRelP2);
		double normalVelAfter = exitVector.normalize() * relativeVelocityAfter;

		// TODO friction again
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
#ifdef CHECK_SANITY
		double beforeTotalEnergy = c.p2->parent->getKineticEnergy();
#endif
		handleCollision(*c.p1, *c.p2, c.intersection, c.exitVector, true);
#ifdef CHECK_SANITY
		double afterTotalEnergy = c.p2->parent->getKineticEnergy();

		if(c.p1->parent->getKineticEnergy() == 0.0 && afterTotalEnergy > beforeTotalEnergy) {
			Log::warn("Energy of blocks after anchored colission is greater than before! %f > %f", afterTotalEnergy, beforeTotalEnergy);
		}
#endif
	}
	for(size_t i = anchoredOffset; i < colissions.size(); i++) {
		Colission c = colissions[i];
#ifdef CHECK_SANITY
		double beforeTotalEnergy = c.p1->parent->getKineticEnergy() + c.p2->parent->getKineticEnergy();
#endif
		handleCollision(*c.p1, *c.p2, c.intersection, c.exitVector, false);
#ifdef CHECK_SANITY
		double afterTotalEnergy = c.p1->parent->getKineticEnergy() + c.p2->parent->getKineticEnergy();
		if(afterTotalEnergy > beforeTotalEnergy) {
			Log::warn("Energy of blocks after colission is greater than before! %f > %f", afterTotalEnergy, beforeTotalEnergy);
		}
#endif
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
		for (AttachedPart& attach : part.parent->parts) {
			if (attach.part == &part) {
				goto partFount;
			}
		}
		Log::error("part's parent's child is not part");
		__debugbreak();
		partFount:
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

double WorldPrototype::getTotalKineticEnergy() const {
	double total = 0.0;
	for(const Physical& p : iterUnAnchoredPhysicals()) {
		total += p.getKineticEnergy();
	}
	return total;
}
double WorldPrototype::getTotalPotentialEnergy() const {
	double total = 0.0;
	for(const Physical& p : iterUnAnchoredPhysicals()) {
		total += getPotentialEnergyOfPhysical(p);
	}
	return total;
}
double WorldPrototype::getPotentialEnergyOfPhysical(const Physical& p) const {
	return 0.0;
}
double WorldPrototype::getTotalEnergy() const {
	return getTotalKineticEnergy() + getTotalPotentialEnergy();
}