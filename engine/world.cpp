
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

PhysicalContainer::PhysicalContainer(size_t initialCapacity) : physicalCount(0), freePhysicalsOffset(0), physicals(nullptr), parts(nullptr) {
	ensureCapacity(initialCapacity);
}
void PhysicalContainer::ensureCapacity(size_t targetCapacity) {
	if(this->capacity < targetCapacity) {
		size_t newCapacity = std::max(this->capacity * 2, static_cast<size_t>(2048));
		Physical* newPhysicals = new Physical[newCapacity];
		Part** newParts = new Part*[newCapacity];

		for(int i = 0; i < this->capacity; i++) {
			newPhysicals[i] = this->physicals[i];
			newParts[i] = this->parts[i];
		}

		delete[] this->physicals;
		delete[] this->parts;
		this->physicals = newPhysicals;
		this->parts = newParts;
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
void PhysicalContainer::add(Part* part, bool anchored) {
	ensureCapacity(physicalCount+1);
	if(anchored) {
		movePhysical(freePhysicalsOffset, physicalCount++);
		part->parent = physicals + freePhysicalsOffset;
		physicals[freePhysicalsOffset++] = Physical(*part);
	} else {
		part->parent = physicals + physicalCount;
		physicals[physicalCount++] = Physical(*part);
	}
}
void PhysicalContainer::remove(size_t index) {
	if(isAnchored(index)) {
		movePhysical(--freePhysicalsOffset, index);
		movePhysical(--physicalCount, freePhysicalsOffset);
	} else {
		movePhysical(--physicalCount, index);
	}
}
void PhysicalContainer::remove(Physical* physical) {
	physicalCount--;
	if(isAnchored(physical)) {
		freePhysicalsOffset--;
		movePhysical(physicals + freePhysicalsOffset, physical);
		movePhysical(physicalCount, freePhysicalsOffset);
	} else {
		movePhysical(physicals + physicalCount, physical);
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
	first->part.parent = first;
	second->part.parent = second;
}
void PhysicalContainer::swapPhysical(size_t first, size_t second) {
	std::swap(physicals[first], physicals[second]);
	physicals[first].part.parent = physicals + first;
	physicals[second].part.parent = physicals + second;
}
void PhysicalContainer::movePhysical(size_t origin, size_t destination) {
	physicals[destination] = physicals[origin];
	physicals[destination].part.parent = &physicals[destination];
}
void PhysicalContainer::movePhysical(Physical* origin, Physical* destination) {
	*destination = *origin;
	destination->part.parent = destination;
}

World::World() : physicals(20) {}


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

		Vec3 frictionForce = -(p1.part.properties.friction + p2.part.properties.friction)/2 * relVelSidewaysComponent * combinedInertia * COLLISSION_RELATIVE_VELOCITY_FORCE_MULTIPLIER;
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

		Vec3 frictionForce = -(anchored.part.properties.friction + freePhys.part.properties.friction) / 2 * relVelSidewaysComponent * combinedInertia * COLLISSION_RELATIVE_VELOCITY_FORCE_MULTIPLIER;
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

			double distanceSqBetween = (anchoredPart.cframe.position - freePart.cframe.position).lengthSquared();

			if(distanceSqBetween > maxRadiusBetween*maxRadiusBetween) {
				intersectionStatistics.addToTally(IntersectionResult::DISTANCE_REJECT, 1);
				continue;
			}

			const Shape& transfFree = freePart.transformed;

			Vec3 intersection;
			Vec3 exitVector;
			if(transfAnchored.intersects(transfFree, intersection, exitVector)) {
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

			double distanceSqBetween = (p1.cframe.position - p2.cframe.position).lengthSquared();

			if(distanceSqBetween > maxRadiusBetween*maxRadiusBetween) {
				intersectionStatistics.addToTally(IntersectionResult::DISTANCE_REJECT, 1);
				continue;
			}

			const Shape& transfI = p1.transformed;
			const Shape& transfJ = p2.transformed;

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
	for(int i = 0; i < physicals.physicalCount; i++) {
		Physical& physical = physicals[i];
		physical.update(deltaT);
	}

	physicsMeasure.mark(PhysicsProcess::OTHER);
	processQueue();
}

size_t World::getTotalVertexCount() {
	size_t total = 0;
	for(const Physical& physical : physicals)
		total += physical.part.hitbox.vertexCount;
	return total;
}

void World::addPartUnsafe(Part* part, bool anchored) {
	physicals.add(part, anchored);
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

void World::addObject(Part* part) {
	return this->addObject(part, false);
};

void World::removePart(Part* part) {
	this->physicals.remove(part->parent);
}

void World::applyExternalForces() {}
