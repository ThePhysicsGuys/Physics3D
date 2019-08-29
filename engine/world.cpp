#include "world.h"


#ifdef CHECK_SANITY
#define ASSERT_VALID if (!isValid()) {throw "World not valid!";}
#else
#define ASSERT_VALID
#endif


WorldPrototype::WorldPrototype() : WorldPrototype(16) {}
WorldPrototype::WorldPrototype(size_t initialPartCapacity) : physicals(initialPartCapacity) {

}


size_t WorldPrototype::getTotalVertexCount() {
	size_t total = 0;
	for (const Physical& phys : iterPhysicals())
		for (const Part& part : phys)
			total += part.hitbox.vertexCount;
	return total;
}

void WorldPrototype::pushOperation(const std::function<void(WorldPrototype*)>& func) {
	std::lock_guard<std::mutex> lg(queueLock);
	waitingOperations.push(func);
}



void WorldPrototype::addPartUnsafe(Part* part, bool anchored) {
	if (part->parent == nullptr) {
		part->parent = new Physical(part);
	}
	part->parent->setAnchored(anchored);
	if (anchored) {
		physicals.addLeftSide(part->parent);
	} else {
		physicals.add(part->parent);
	}
	objectTree.add(part, part->getStrictBounds());
	ASSERT_VALID;
}
void WorldPrototype::removePartUnsafe(Part* part) {
	Physical* parent = part->parent;
	objectTree.remove(part);
	if (parent->detachPart(part)) {
		physicals.remove(parent);
	}
	
	ASSERT_VALID;
}
void WorldPrototype::attachPartUnsafe(Part* part, Physical& phys, CFrame attachment) {
	if (part->parent != nullptr) {
		removePartUnsafe(part);
	}
	phys.attachPart(part, attachment);
	objectTree.addToExistingGroup(part, part->getStrictBounds(), phys.mainPart, phys.mainPart->getStrictBounds());
	ASSERT_VALID;
}

void WorldPrototype::detachPartUnsafe(Part* part) {
	removePartUnsafe(part);
	addPartUnsafe(part, false);
	ASSERT_VALID;
}

void WorldPrototype::processQueue() {
	std::lock_guard<std::mutex> lg(queueLock);

	while (!waitingOperations.empty()) {
		std::function<void(WorldPrototype*)>& operation = waitingOperations.front();
		operation(this);
		waitingOperations.pop();
	}
}


void WorldPrototype::addPart(Part* part, bool anchored) {
	if (lock.try_lock()) {
		addPartUnsafe(part, anchored);
		lock.unlock();
	} else {
		pushOperation([part, anchored](WorldPrototype* world) {world->addPartUnsafe(part, anchored); });
	}
}
void WorldPrototype::removePart(Part* part) {
	if (lock.try_lock()) {
		removePartUnsafe(part);
		lock.unlock();
	} else {
		pushOperation([part](WorldPrototype* world) {world->removePartUnsafe(part); });
	}
}
void WorldPrototype::attachPart(Part* part, Physical& phys, CFrame attachment) {
	if (lock.try_lock()) {
		attachPartUnsafe(part, phys, attachment);
		lock.unlock();
	} else {
		pushOperation([part, &phys, attachment](WorldPrototype* world) {world->attachPartUnsafe(part, phys, attachment); });
	}
}
void WorldPrototype::detachPart(Part* part) {
	if (lock.try_lock()) {
		detachPartUnsafe(part);
		lock.unlock();
	} else {
		pushOperation([part](WorldPrototype* world) {world->detachPartUnsafe(part); });
	}
}

void recursiveTreeValidCheck(const TreeNode& node) {
	if (node.isLeafNode()) return;
	
	Bounds bounds = node[0].bounds;
	for (int i = 1; i < node.nodeCount; i++) {
		bounds = unionOfBounds(bounds, node[i].bounds);
	}
	if (bounds != node.bounds) {
		throw "A node in the tree does not have valid bounds!";
	}

	for (TreeNode& n : node) {
		recursiveTreeValidCheck(n);
	}
}

bool WorldPrototype::isValid() const {
	for (const Physical& phys : iterPhysicals()) {
		for (const Part& part : phys) {
			if (part.parent != &phys) {
				Log::error("part's parent's child is not part");
				__debugbreak();
				return false;
			}
		}
	}

	recursiveTreeValidCheck(objectTree.rootNode);

	return true;
}
