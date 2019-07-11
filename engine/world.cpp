
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

#ifdef CHECK_SANITY
#define ASSERT_VALID if (!isValid()) {throw "World not valid!";}
#else
#define ASSERT_VALID
#endif

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
template<bool anchoredColission>
void handleCollision(Part& part1, Part& part2, Vec3 collisionPoint, Vec3 exitVector) {
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

	// Friction
	double staticFriction = part1.properties.friction * part2.properties.friction;
	double dynamicFriction = part1.properties.friction * part2.properties.friction;

	
	Vec3 depthForce = -exitVector * (COLLISSION_DEPTH_FORCE_MULTIPLIER * combinedInertia);

	if(!anchoredColission) p1.applyForce(collissionRelP1, depthForce);
	p2.applyForce(collissionRelP2, -depthForce);


	Vec3 relativeVelocity = p1.getVelocityOfPoint(collissionRelP1) - p2.getVelocityOfPoint(collissionRelP2);

	bool isImpulseColission = relativeVelocity * exitVector > 0;

	Vec3 impulse;

	if(isImpulseColission) { // moving towards the other object
		Vec3 desiredAccel = -exitVector * (relativeVelocity * exitVector) / exitVector.lengthSquared();
		Vec3 zeroRelVelImpulse = desiredAccel * combinedInertia;
		impulse = zeroRelVelImpulse * (1.0+ELASTICITY);
		if(!anchoredColission) p1.applyImpulse(collissionRelP1, impulse);
		p2.applyImpulse(collissionRelP2, -impulse);
		relativeVelocity = p1.getVelocityOfPoint(collissionRelP1) - p2.getVelocityOfPoint(collissionRelP2); // set new relativeVelocity
	}

	Vec3 slidingVelocity = exitVector % relativeVelocity % exitVector / exitVector.lengthSquared();

	// Compute combined inertia in the horizontal direction
	double combinedHorizontalInertia;
	if (anchoredColission)
		combinedHorizontalInertia = p2.getInertiaOfPointInDirectionRelative(collissionRelP2, slidingVelocity);
	else {
		double inertia1 = p1.getInertiaOfPointInDirectionRelative(collissionRelP1, slidingVelocity);
		double inertia2 = p2.getInertiaOfPointInDirectionRelative(collissionRelP2, slidingVelocity);
		combinedHorizontalInertia = 1 / (1 / inertia1 + 1 / inertia2);
	}

	if (isImpulseColission) {
		Vec3 maxFrictionImpulse = -exitVector % impulse % exitVector / exitVector.lengthSquared() * staticFriction;
		Vec3 stopFricImpulse = -slidingVelocity * combinedHorizontalInertia;

		Vec3 fricImpulse = (stopFricImpulse.lengthSquared() < maxFrictionImpulse.lengthSquared()) ? stopFricImpulse : maxFrictionImpulse;

		if (!anchoredColission) p1.applyImpulse(collissionRelP1, fricImpulse);
		p2.applyImpulse(collissionRelP2, -fricImpulse);
	}

	double normalForce = depthForce.length();
	double frictionForce = normalForce * dynamicFriction;
	double slidingSpeed = slidingVelocity.length() + 1E-100;
	Vec3 dynamicFricForce;
	double dynamicSaturationSpeed = sizeOrder * 0.01;
	if (slidingSpeed > dynamicSaturationSpeed) {
		dynamicFricForce = -slidingVelocity / slidingSpeed * frictionForce;
	} else {
		double effectFactor = slidingSpeed / (dynamicSaturationSpeed);
		dynamicFricForce = -slidingVelocity / slidingSpeed * frictionForce * effectFactor;
	}
	if (!anchoredColission) p1.applyForce(collissionRelP1, dynamicFricForce);
	p2.applyForce(collissionRelP2, -dynamicFricForce);
}

struct Colission {
	Part* p1;
	Part* p2;
	Vec3 intersection;
	Vec3 exitVector;
};

bool boundsSphereEarlyEnd(const BoundingBox& bounds, const Vec3& localSphereCenter, double sphereRadius) {
	const Vec3& sphere = localSphereCenter;

	BoundingBox expandedBounds = bounds.expanded(sphereRadius);

	if (expandedBounds.containsPoint(localSphereCenter)) {
		return false;
	} else {
		return true;
	}
}

inline void runColissionTests(Physical& phys1, Physical& phys2, WorldPrototype& world, std::vector<Colission>& colissions) {
	Vec3 deltaCentroid = phys1.circumscribingSphere.origin - phys2.circumscribingSphere.origin;
	double maxDistanceBetween = phys1.circumscribingSphere.radius + phys2.circumscribingSphere.radius;
	if (deltaCentroid.lengthSquared() > maxDistanceBetween * maxDistanceBetween) {
		intersectionStatistics.addToTally(IntersectionResult::PHYSICAL_DISTANCE_REJECT, phys1.getPartCount() * phys2.getPartCount());
		return;
	}
	if (boundsSphereEarlyEnd(phys1.localBounds, phys1.getCFrame().globalToLocal(phys2.circumscribingSphere.origin), phys2.circumscribingSphere.radius)) {
		intersectionStatistics.addToTally(IntersectionResult::PHYSICAL_BOUNDS_REJECT, phys2.getPartCount() * phys1.getPartCount());
		return;
	}
	if (boundsSphereEarlyEnd(phys2.localBounds, phys2.getCFrame().globalToLocal(phys1.circumscribingSphere.origin), phys1.circumscribingSphere.radius)) {
		intersectionStatistics.addToTally(IntersectionResult::PHYSICAL_BOUNDS_REJECT, phys2.getPartCount() * phys1.getPartCount());
		return;
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
			if (boundsSphereEarlyEnd(p1.localBounds, p1.cframe.globalToLocal(p2.cframe.getPosition()), p2.maxRadius)) {
				intersectionStatistics.addToTally(IntersectionResult::PART_BOUNDS_REJECT, 1);
				continue;
			}
			if (boundsSphereEarlyEnd(p2.localBounds, p2.cframe.globalToLocal(p1.cframe.getPosition()), p1.maxRadius)) {
				intersectionStatistics.addToTally(IntersectionResult::PART_BOUNDS_REJECT, 1);
				continue;
			}

			Vec3 intersection;
			Vec3 exitVector;
			if (p1.intersects(p2, intersection, exitVector)) {
				intersectionStatistics.addToTally(IntersectionResult::COLISSION, 1);
				colissions.push_back(Colission{ &p1, &p2, intersection, exitVector });
			} else {
				intersectionStatistics.addToTally(IntersectionResult::GJK_REJECT, 1);
			}
			physicsMeasure.mark(PhysicsProcess::COLISSION_OTHER);
		}
	}
}

// returns anchoredColissions offset
size_t findColissions(WorldPrototype& world, std::vector<Colission>& colissions) {
	for(Physical& anchoredPhys: world.iterAnchoredPhysicals()) {
		for(Physical& freePhys: world.iterFreePhysicals()) {
			runColissionTests(anchoredPhys, freePhys, world, colissions);
		}
	}

	size_t anchoredOffset = colissions.size();

	auto iter = world.iterFreePhysicals();
	auto finish = iter.end();
	for(auto mainIter = iter.begin(); mainIter != finish; ++mainIter) {
		Physical& phys1 = *mainIter;
		for(auto secondIter = mainIter; ++secondIter != finish; ) {
			runColissionTests(phys1, *secondIter, world, colissions);
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
		handleCollision<true>(*c.p1, *c.p2, c.intersection, c.exitVector);
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
		handleCollision<false>(*c.p1, *c.p2, c.intersection, c.exitVector);
#ifdef CHECK_SANITY
		double afterTotalEnergy = c.p1->parent->getKineticEnergy() + c.p2->parent->getKineticEnergy();
		if(afterTotalEnergy > beforeTotalEnergy) {
			Log::warn("Energy of blocks after colission is greater than before! %f > %f", afterTotalEnergy, beforeTotalEnergy);
		}
#endif
	}


	intersectionStatistics.nextTally();

	physicsMeasure.mark(PhysicsProcess::WAIT_FOR_LOCk);
	mutLock.upgrade();
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

void WorldPrototype::pushOperation(const std::function<void(WorldPrototype*)>& func) {
	std::lock_guard<std::mutex> lg(queueLock);
	waitingOperations.push(func);
}

void WorldPrototype::addPartUnsafe(Part* part, bool anchored) {
	Physical* phys = new Physical(part);
	if (anchored) {
		physicals.addLeftSide(phys);
	} else {
		physicals.add(phys);
	}
	objectTree.add(phys);
	ASSERT_VALID;
}
void WorldPrototype::removePartUnsafe(Part* part) {
	Physical* parent = part->parent;
	parent->detachPart(part);
	if (parent->getPartCount() == 0) {
		throw "TODO fix";
		//physicals.remove(parent);
	}
	ASSERT_VALID;
}
void WorldPrototype::attachPartUnsafe(Part* part, Physical& phys, CFrame attachment) {
	if (part->parent != nullptr) {
		removePartUnsafe(part);
	}
	phys.attachPart(part, attachment);
	ASSERT_VALID;
}

void WorldPrototype::detachPartUnsafe(Part* part) {
	removePartUnsafe(part);
	addPartUnsafe(part, false);
	ASSERT_VALID;
}

void WorldPrototype::processQueue() {
	std::lock_guard<std::mutex> lg(queueLock);

	while(!waitingOperations.empty()) {
		std::function<void(WorldPrototype*)>& operation = waitingOperations.front();
		operation(this);
		waitingOperations.pop();
	}
}


void WorldPrototype::addPart(Part* part, bool anchored) {
	if(lock.try_lock()) {
		addPartUnsafe(part, anchored);
		lock.unlock();
	} else {
		pushOperation([part, anchored](WorldPrototype * world) {world->addPartUnsafe(part, anchored); });
	}
}
void WorldPrototype::removePart(Part* part) {
	if (lock.try_lock()) {
		removePartUnsafe(part);
		lock.unlock();
	} else {
		pushOperation([part](WorldPrototype * world) {world->removePartUnsafe(part); });
	}
}
void WorldPrototype::attachPart(Part* part, Physical& phys, CFrame attachment) {
	if (lock.try_lock()) {
		attachPartUnsafe(part, phys, attachment);
		lock.unlock();
	} else {
		pushOperation([part, &phys, attachment](WorldPrototype * world) {world->attachPartUnsafe(part, phys, attachment); });
	}
}
void WorldPrototype::detachPart(Part* part) {
	if (lock.try_lock()) {
		detachPartUnsafe(part);
		lock.unlock();
	} else {
		pushOperation([part](WorldPrototype * world) {world->detachPartUnsafe(part); });
	}
}


void WorldPrototype::applyExternalForces() {}

bool WorldPrototype::isValid() const {
	for (const Physical*const * phys = this->iterPhysicals().start; phys != this->iterPhysicals().fin; phys++) {
		for (const Part& part : **phys) {
			if (part.parent != *phys) {
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