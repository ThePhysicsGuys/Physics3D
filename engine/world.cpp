
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

WorldPrototype::WorldPrototype() : WorldPrototype(16) {}
WorldPrototype::WorldPrototype(size_t initialPartCapacity) : physicals(initialPartCapacity) {
	
}

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
		combinedInertia = p2.getInertiaOfPointInDirectionRelative(collissionRelP2, exitVector);
	else {
		double inertia1 = p1.getInertiaOfPointInDirectionRelative(collissionRelP1, exitVector);
		double inertia2 = p2.getInertiaOfPointInDirectionRelative(collissionRelP2, exitVector);
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
	for(Physical& anchoredPhys: world.iterAnchoredPhysicals()) {
		for(Physical& freePhys: world.iterFreePhysicals()) {
			Vec3 deltaPosition = freePhys.getCFrame().getPosition() - anchoredPhys.getCFrame().getPosition();
			double maxDistanceBetween = freePhys.maxRadius + anchoredPhys.maxRadius;
			if (deltaPosition.lengthSquared() > maxDistanceBetween * maxDistanceBetween) {
				intersectionStatistics.addToTally(IntersectionResult::PHYSICAL_DISTANCE_REJECT, freePhys.getPartCount() * anchoredPhys.getPartCount());
				continue;
			}
			for (Part& anchoredPart : anchoredPhys) {
				for (Part& freePart : freePhys) {
					double maxRadiusBetween = anchoredPart.maxRadius + freePart.maxRadius;

					Vec3 deltaPosition = anchoredPart.cframe.position - freePart.cframe.position;
					double distanceSqBetween = deltaPosition.lengthSquared();

					if (distanceSqBetween > maxRadiusBetween * maxRadiusBetween) {
						intersectionStatistics.addToTally(IntersectionResult::PART_DISTANCE_REJECT, 1);
						continue;
					}

					Vec3 intersection;
					Vec3 exitVector;
					if (anchoredPart.intersects(freePart, intersection, exitVector)) {
						intersectionStatistics.addToTally(IntersectionResult::COLISSION, 1);
						colissions.push_back(Colission{ &anchoredPart, &freePart, intersection, exitVector });
					}
					else {
						intersectionStatistics.addToTally(IntersectionResult::GJK_REJECT, 1);
					}
					physicsMeasure.mark(PhysicsProcess::COLISSION_OTHER);
				}
			}
			
		}
	}

	size_t anchoredOffset = colissions.size();

	auto iter = world.iterFreePhysicals();
	auto finish = iter.end();
	for(auto mainIter = iter.begin(); mainIter != finish; ++mainIter) {
		Physical& phys1 = *mainIter;
		for(auto secondIter = mainIter; ++secondIter != finish; ) {
			Physical& phys2 = *secondIter;
			Vec3 deltaPosition = phys1.getCFrame().getPosition() - phys2.getCFrame().getPosition();
			double maxDistanceBetween = phys1.maxRadius + phys2.maxRadius;
			if (deltaPosition.lengthSquared() > maxDistanceBetween * maxDistanceBetween) {
				intersectionStatistics.addToTally(IntersectionResult::PHYSICAL_DISTANCE_REJECT, phys1.getPartCount()*phys2.getPartCount());
				continue;
			}
			for (Part& p1 : phys1) {
				for (Part& p2 : phys2) {
					double maxRadiusBetween = p1.maxRadius + p2.maxRadius;

					Vec3 deltaPosition = p1.cframe.position - p2.cframe.position;
					double distanceSqBetween = deltaPosition.lengthSquared();

					if (distanceSqBetween > maxRadiusBetween * maxRadiusBetween) {
						intersectionStatistics.addToTally(IntersectionResult::PART_DISTANCE_REJECT, 1);
						continue;
					}

					Vec3 intersection;
					Vec3 exitVector;
					if (p1.intersects(p2, intersection, exitVector)) {
						intersectionStatistics.addToTally(IntersectionResult::COLISSION, 1);
						colissions.push_back(Colission{ &p1, &p2, intersection, exitVector });
					}
					else {
						intersectionStatistics.addToTally(IntersectionResult::GJK_REJECT, 1);
					}
					physicsMeasure.mark(PhysicsProcess::COLISSION_OTHER);
				}
			}
		}
	}

	return anchoredOffset;
}


void WorldPrototype::tick(double deltaT) {
	SharedLockGuard mutLock(lock);
#ifdef USE_TRANSFORMATIONS
	physicsMeasure.mark(PhysicsProcess::TRANSFORMS);
	std::shared_ptr<Vec3f> vecBuf(new Vec3f[getTotalVertexCount()], std::default_delete<Vec3f[]>());
	Vec3f* vecBufIndex = vecBuf.get();
	for(Physical& phys : iterPhysicals()) {
		for (Part& part : phys) {
			const Shape& curShape = part.hitbox;
			CFrame cframe = part.cframe;
			part.transformed = curShape.localToGlobal(cframe, vecBufIndex);
			vecBufIndex += curShape.vertexCount;
		}
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
	for(const Physical& phys : iterPhysicals())
		for (const Part& part : phys)
			total += part.hitbox.vertexCount;
	return total;
}

void WorldPrototype::addPartUnsafe(Part* part, bool anchored) {
	if (anchored) {
		physicals.addLeftSide(Physical(part));
	} else {
		physicals.add(Physical(part));
	}

	if (!isValid()) {
		throw "World not valid!";
	}
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
}
void WorldPrototype::attachPart(Part* p, Physical& phys, CFrame attachment) {
	if (p->parent != nullptr) {
		removePart(p);
	}
	phys.attachPart(p, attachment);
	// addPartUnsafe(p, false);
}

void WorldPrototype::removePart(Part* part) {
	Physical* parent = part->parent;
	parent->detachPart(part);
	if (parent->getPartCount() == 0) {
		physicals.remove(parent);
	}
}

void WorldPrototype::applyExternalForces() {}

bool WorldPrototype::isValid() const {
	for (const Physical* phys = this->iterPhysicals().begin(); phys != this->iterPhysicals().end(); phys++) {
		for (const Part& part : *phys) {
			if (part.parent != phys) {
				Log::error("part's parent's child is not part");
				__debugbreak();
				return false;
			}
		}
	}
	
	return true;
}

double WorldPrototype::getTotalKineticEnergy() const {
	double total = 0.0;
	for(const Physical& p : iterFreePhysicals()) {
		total += p.getKineticEnergy();
	}
	return total;
}
double WorldPrototype::getTotalPotentialEnergy() const {
	double total = 0.0;
	for(const Physical& p : iterFreePhysicals()) {
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