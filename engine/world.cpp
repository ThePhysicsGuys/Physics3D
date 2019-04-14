
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

PhysicalContainer::PhysicalContainer(size_t initialCapacity) : physicalCount(0), freePhysicalsOffset(0), physicals(nullptr) {
	ensureCapacity(initialCapacity);
}
void PhysicalContainer::ensureCapacity(size_t targetCapacity) {
	if(this->capacity < targetCapacity) {
		size_t newCapacity = std::max(static_cast<unsigned long long>(10), this->capacity * 2);
		Physical* newPhysicals = new Physical[newCapacity];
		for(int i = 0; i < this->capacity; i++) {
			newPhysicals[i] = this->physicals[i];
		}

		delete[] this->physicals;
		this->physicals = newPhysicals;
		this->capacity = newCapacity;
		Log::info("Extended physicals capacity to %d", newCapacity);
	}
}
void PhysicalContainer::add(Physical& p) {
	ensureCapacity(physicalCount+1);
	physicals[physicalCount++] = p;
}
void PhysicalContainer::addAnchored(Physical& p) {
	ensureCapacity(physicalCount+1);
	movePhysical(freePhysicalsOffset, physicalCount++);
	physicals[freePhysicalsOffset++] = p;
}
void PhysicalContainer::remove(size_t index) {
	if(isAnchored(index)) {
		movePhysical(--freePhysicalsOffset, index);
		movePhysical(--physicalCount, freePhysicalsOffset);
	} else {
		movePhysical(--physicalCount, index);
	}
}
void PhysicalContainer::anchor(size_t index) {
	if(!isAnchored(index)) {
		swapPhysical(index, freePhysicalsOffset++);
	}
}
void PhysicalContainer::anchor(Physical* p) {
	if(!isAnchored(p)) {
		swapPhysical(p, physicals + freePhysicalsOffset);
		freePhysicalsOffset++;
	}
}
void PhysicalContainer::unanchor(size_t index) {
	if(isAnchored(index)) {
		swapPhysical(index, --freePhysicalsOffset);
	}
}
void PhysicalContainer::unanchor(Physical* p) {
	if(isAnchored(p)) {
		freePhysicalsOffset--;
		swapPhysical(p, physicals + freePhysicalsOffset);
	}
}
bool PhysicalContainer::isAnchored(size_t index) {
	return index < freePhysicalsOffset;
}
bool PhysicalContainer::isAnchored(const Physical* physical) const {
	return physical < physicals + freePhysicalsOffset;
}
void PhysicalContainer::swapPhysical(Physical* first, Physical* second) {
	std::swap(*first, *second);
}
void PhysicalContainer::swapPhysical(size_t first, size_t second) {
	std::swap(physicals[first], physicals[second]);
}
void PhysicalContainer::movePhysical(size_t origin, size_t destination) {
	physicals[destination] = physicals[origin];
}

World::World() : physicals(20) {}


typedef Physical AnchoredPhysical;

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

/*
exitVector is the distance the free physical must travel so that the shapes are no longer colliding
*/
void handleAnchoredCollision(AnchoredPhysical& anchored, Physical& freePhys, Vec3 collisionPoint, Vec3 exitVector) {

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

		Vec3 frictionForce = -(anchored.part.properties.friction + freePhys.part.properties.friction) / 2 * relVelSidewaysComponent * combinedInertia * COLLISSION_RELATIVE_VELOCITY_FORCE_MULTIPLIER;
		// anchored.applyForce(collissionRelP1, frictionForce);
		freePhys.applyForce(collissionRelP2, -frictionForce);
	}
}

void World::processQueue() {
	std::lock_guard<std::mutex> lg(queueLock);

	while(!newPhysicalQueue.empty()) {
		QueuedPhysical& qp = newPhysicalQueue.front();
		if(qp.anchored) {
			physicals.addAnchored(qp.p);
		} else {
			physicals.add(qp.p);
		}
		newPhysicalQueue.pop();
	}
}

void World::processColissions(const Shape* transformedShapes) {
	physicsMeasure.mark(PhysicsProcess::COLISSION_OTHER);

	for(int i = 0; i < physicals.freePhysicalsOffset; i++) {
		AnchoredPhysical& anchoredPhys = physicals[i];
		const Shape& transfAnchored = transformedShapes[i];
		for(int j = physicals.freePhysicalsOffset; j < physicals.physicalCount; j++) {
			Physical& freePhys = physicals[j];

			double maxRadiusBetween = anchoredPhys.maxRadius + freePhys.maxRadius;

			double distanceSqBetween = (anchoredPhys.getCenterOfMass() - freePhys.getCenterOfMass()).lengthSquared();

			if(distanceSqBetween > maxRadiusBetween*maxRadiusBetween) {
				intersectionStatistics.addToTally(IntersectionResult::DISTANCE_REJECT, 1);
				continue;
			}

			const Shape& transfFree = transformedShapes[j];

			Vec3 intersection;
			Vec3 exitVector;
			if(transfAnchored.intersects(transfFree, intersection, exitVector)) {
				physicsMeasure.mark(PhysicsProcess::COLISSION_HANDLING);
				intersectionStatistics.addToTally(IntersectionResult::COLISSION, 1);
				handleAnchoredCollision(anchoredPhys, freePhys, intersection, exitVector);
			} else {
				physicsMeasure.mark(PhysicsProcess::COLISSION_OTHER);
				intersectionStatistics.addToTally(IntersectionResult::GJK_REJECT, 1);
			}
		}
	}

	for(int i = physicals.freePhysicalsOffset; i < physicals.physicalCount; i++) {
		Physical& p1 = physicals[i];
		const Shape& transfI = transformedShapes[i];
		for(int j = i + 1; j < physicals.physicalCount; j++) {
			Physical& p2 = physicals[j];

			double maxRadiusBetween = p1.maxRadius + p2.maxRadius;

			double distanceSqBetween = (p1.getCenterOfMass() - p2.getCenterOfMass()).lengthSquared();

			if(distanceSqBetween > maxRadiusBetween*maxRadiusBetween) {
				intersectionStatistics.addToTally(IntersectionResult::DISTANCE_REJECT, 1);
				continue;
			}

			const Shape& transfJ = transformedShapes[j];

			Vec3 intersection;
			Vec3 exitVector;
			if(transfI.intersects(transfJ, intersection, exitVector)) {
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

	Vec3* vecBuf = (Vec3*) alloca(getTotalVertexCount() * sizeof(Vec3));
	Shape* transformedShapes = new Shape[physicals.physicalCount];

	Vec3* vecBufIndex = vecBuf;

	physicsMeasure.mark(PhysicsProcess::TRANSFORMS);
	for(int i = 0; i < physicals.physicalCount; i++) {
		const Shape& curShape = physicals[i].part.hitbox;
		CFrame cframe = physicals[i].part.cframe;
		transformedShapes[i] = curShape.localToGlobal(cframe, vecBufIndex);
		vecBufIndex += curShape.vertexCount;
	}

	physicsMeasure.mark(PhysicsProcess::EXTERNALS);
	applyExternalForces(transformedShapes);

	// Compute object collisions

	processColissions(transformedShapes);

	intersectionStatistics.nextTally();

	physicsMeasure.mark(PhysicsProcess::UPDATING);
	mutLock.upgrade();
	for(int i = 0; i < physicals.physicalCount; i++) {
		Physical& physical = physicals[i];
		physical.update(deltaT);
	}

	physicsMeasure.mark(PhysicsProcess::OTHER);
	delete[] transformedShapes;
	processQueue();
}

size_t World::getTotalVertexCount() {
	size_t total = 0;
	for(const Physical& physical : physicals)
		total += physical.part.hitbox.vertexCount;
	return total;
}

void World::addObject(Physical& part, bool anchored) {
	if(lock.try_lock()) {
		if(anchored) {
			physicals.addAnchored(part);
		} else {
			physicals.add(part);
		}
		lock.unlock();
	} else {
		std::lock_guard<std::mutex> lg(queueLock);
		newPhysicalQueue.push(QueuedPhysical{part, anchored });
	}
}

Physical& World::addObject(Part& part, bool anchored) {
	Physical physical(part);
	this->addObject(physical, anchored);
	return physical;
};

Physical& World::addObject(Part& part) {
	return this->addObject(part, false);
};

Physical& World::addObject(Shape shape, CFrame location, double density, double friction, bool anchored) {
	Part part(shape, location, density, friction);
	Physical newPhysical(part);
	addObject(newPhysical, anchored);

	return newPhysical;
};

void World::applyExternalForces(const Shape* transformedShapes) {}

