
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
	exitVector is the distance p2 must travel so that the shapes are no longer colliding
*/
template<bool anchoredColission>
void handleCollision(Part& part1, Part& part2, Position collisionPoint, Vec3 exitVector) {
	Debug::logPoint(collisionPoint, Debug::INTERSECTION);
	Physical& p1 = *part1.parent;
	Physical& p2 = *part2.parent;

	double sizeOrder = std::min(part1.maxRadius, part2.maxRadius);
	if(lengthSquared(exitVector) <= 1E-8 * sizeOrder*sizeOrder) {
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
		Vec3 desiredAccel = -exitVector * (relativeVelocity * exitVector) / lengthSquared(exitVector);
		Vec3 zeroRelVelImpulse = desiredAccel * combinedInertia;
		impulse = zeroRelVelImpulse * (1.0+ELASTICITY);
		if(!anchoredColission) p1.applyImpulse(collissionRelP1, impulse);
		p2.applyImpulse(collissionRelP2, -impulse);
		relativeVelocity = p1.getVelocityOfPoint(collissionRelP1) - p2.getVelocityOfPoint(collissionRelP2); // set new relativeVelocity
	}

	Vec3 slidingVelocity = exitVector % relativeVelocity % exitVector / lengthSquared(exitVector);

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
		Vec3 maxFrictionImpulse = -exitVector % impulse % exitVector / lengthSquared(exitVector) * staticFriction;
		Vec3 stopFricImpulse = -slidingVelocity * combinedHorizontalInertia;

		Vec3 fricImpulse = (lengthSquared(stopFricImpulse) < lengthSquared(maxFrictionImpulse)) ? stopFricImpulse : maxFrictionImpulse;

		if (!anchoredColission) p1.applyImpulse(collissionRelP1, fricImpulse);
		p2.applyImpulse(collissionRelP2, -fricImpulse);
	}

	double normalForce = length(depthForce);
	double frictionForce = normalForce * dynamicFriction;
	double slidingSpeed = length(slidingVelocity) + 1E-100;
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
	Position intersection;
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
	if (lengthSquared(deltaCentroid) > maxDistanceBetween * maxDistanceBetween) {
		intersectionStatistics.addToTally(IntersectionResult::PHYSICAL_DISTANCE_REJECT, phys1.getPartCount() * phys2.getPartCount());
		return;
	}
	if (phys1.anchored && phys2.anchored) return;
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
			double distanceSqBetween = lengthSquared(deltaPosition);

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

			Position intersection;
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

void recursiveFindColissionsInternal(WorldPrototype& world, std::vector<Colission>& colissions, TreeNode& trunkNode);
void recursiveFindColissionsBetween(WorldPrototype& world, std::vector<Colission>& colissions, TreeNode& first, TreeNode& second);

void findColissions(WorldPrototype& world, std::vector<Colission>& colissions) {
	TreeNode& node = world.objectTree.rootNode;
	
	// two kinds of collissions: intra-node, and inter-node

	recursiveFindColissionsInternal(world, colissions, node);
}

void recursiveFindColissionsInternal(WorldPrototype& world, std::vector<Colission>& colissions, TreeNode& trunkNode) {
	// within the same node
	if (trunkNode.isLeafNode())
		return;

	for (int i = 0; i < trunkNode.nodeCount; i++) {
		TreeNode& A = trunkNode[i];
		recursiveFindColissionsInternal(world, colissions, A);
		for (int j = i + 1; j < trunkNode.nodeCount; j++) {
			TreeNode& B = trunkNode[j];
			recursiveFindColissionsBetween(world, colissions, A, B);
		}
	}
}

void recursiveFindColissionsBetween(WorldPrototype& world, std::vector<Colission>& colissions, TreeNode& first, TreeNode& second) {
	if (!intersects(first.bounds, second.bounds)) return;
	
	if (first.isLeafNode() && second.isLeafNode()) {
		runColissionTests(*static_cast<Physical*>(first.object), *static_cast<Physical*>(second.object), world, colissions);
	} else {
		bool preferFirst = computeCost(first.bounds) <= computeCost(second.bounds);
		if (preferFirst && !first.isLeafNode() || second.isLeafNode()) {
			// split first

			for (TreeNode& node : first) {
				recursiveFindColissionsBetween(world, colissions, node, second);
			}
		} else {
			// split second

			for (TreeNode& node : second) {
				recursiveFindColissionsBetween(world, colissions, first, node);
			}
		}
	}
}


void WorldPrototype::tick(double deltaT) {
	SharedLockGuard mutLock(lock);

	physicsMeasure.mark(PhysicsProcess::EXTERNALS);
	applyExternalForces();

	physicsMeasure.mark(PhysicsProcess::COLISSION_OTHER);
	std::vector<Colission> colissions;

	findColissions(*this, colissions);
	physicsMeasure.mark(PhysicsProcess::COLISSION_HANDLING);
	for (int i = 0; i < colissions.size(); i++) {
		Colission c = colissions[i];
		handleCollision<false>(*c.p1, *c.p2, c.intersection, c.exitVector);
	}

	intersectionStatistics.nextTally();

	physicsMeasure.mark(PhysicsProcess::CONSTRAINTS);
	for (const ConstraintGroup& group : constraints) {
		group.apply();
	}

	physicsMeasure.mark(PhysicsProcess::WAIT_FOR_LOCK);
	mutLock.upgrade();
	physicsMeasure.mark(PhysicsProcess::UPDATING);
	for(Physical& physical : iterPhysicals()) {
		physical.update(deltaT);
	}

	physicsMeasure.mark(PhysicsProcess::UPDATE_TREE_BOUNDS);
	objectTree.recalculateBounds(true);
	physicsMeasure.mark(PhysicsProcess::UPDATE_TREE_STRUCTURE);
	objectTree.improveStructure();

	physicsMeasure.mark(PhysicsProcess::OTHER);
	processQueue();

	age++;
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
	phys->setAnchored(anchored);
	if (anchored) {
		physicals.addLeftSide(phys);
	} else {
		physicals.add(phys);
	}
	objectTree.add(phys, true);
	ASSERT_VALID;
}
void WorldPrototype::removePartUnsafe(Part* part) {
	Physical* parent = part->parent;
	if (parent->detachPart(part)) {
		objectTree.remove(parent);
		physicals.remove(parent);
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
	for (const Physical& phys: iterPhysicals()) {
		for (const Part& part : phys) {
			if (part.parent != &phys) {
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