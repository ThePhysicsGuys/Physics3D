
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

inline void runColissionTests(Part& p1, Part& p2, WorldPrototype& world, std::vector<Colission>& colissions) {
	if (p1.parent->anchored && p2.parent->anchored) return;
	
	double maxRadiusBetween = p1.maxRadius + p2.maxRadius;

	Vec3 deltaPosition = p1.getPosition() - p2.getPosition();
	double distanceSqBetween = lengthSquared(deltaPosition);

	if (distanceSqBetween > maxRadiusBetween * maxRadiusBetween) {
		intersectionStatistics.addToTally(IntersectionResult::PART_DISTANCE_REJECT, 1);
		return;
	}
	if (boundsSphereEarlyEnd(p1.localBounds, p1.getCFrame().globalToLocal(p2.getPosition()), p2.maxRadius)) {
		intersectionStatistics.addToTally(IntersectionResult::PART_BOUNDS_REJECT, 1);
		return;
	}
	if (boundsSphereEarlyEnd(p2.localBounds, p2.getCFrame().globalToLocal(p1.getPosition()), p1.maxRadius)) {
		intersectionStatistics.addToTally(IntersectionResult::PART_BOUNDS_REJECT, 1);
		return;
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

void recursiveFindColissionsInternal(WorldPrototype& world, std::vector<Colission>& colissions, TreeNode& trunkNode);
void recursiveFindColissionsBetween(WorldPrototype& world, std::vector<Colission>& colissions, TreeNode& first, TreeNode& second);

void findColissions(WorldPrototype& world, std::vector<Colission>& colissions) {
	TreeNode& node = world.objectTree.rootNode;
	
	// two kinds of collissions: intra-node, and inter-node

	recursiveFindColissionsInternal(world, colissions, node);
}

void recursiveFindColissionsInternal(WorldPrototype& world, std::vector<Colission>& colissions, TreeNode& trunkNode) {
	// within the same node
	if (trunkNode.isLeafNode() || trunkNode.isGroupHead)
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
		runColissionTests(*static_cast<Part*>(first.object), *static_cast<Part*>(second.object), world, colissions);
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
	objectTree.recalculateBounds();
	physicsMeasure.mark(PhysicsProcess::UPDATE_TREE_STRUCTURE);
	objectTree.improveStructure();

	physicsMeasure.mark(PhysicsProcess::OTHER);
	processQueue();

	age++;
}


void WorldPrototype::applyExternalForces() {}

double WorldPrototype::getTotalKineticEnergy() const {
	double total = 0.0;
	for(const Physical& p : iterPhysicals()) {
		if (p.anchored) continue;
		total += p.getKineticEnergy();
	}
	return total;
}
double WorldPrototype::getTotalPotentialEnergy() const {
	double total = 0.0;
	for(const Physical& p : iterPhysicals()) {
		if (p.anchored) continue;
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