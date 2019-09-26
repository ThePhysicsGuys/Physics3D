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

void WorldPrototype::pushOperation(const std::function<void(WorldPrototype&)>& func) {
	std::lock_guard<std::mutex> lg(queueLock);
	waitingOperations.push(func);
}

void WorldPrototype::requestModification(const std::function<void(WorldPrototype&)>& function) {
	if (lock.try_lock()) {
		function(*this);
		lock.unlock();
	} else {
		pushOperation(function);
	}
}

void WorldPrototype::addPartUnsafe(Part* part, bool anchored) {
	if (part->parent == nullptr) {
		part->parent = new Physical(part);
		physicals.add(part->parent);
		objectTree.add(part, part->getStrictBounds());
	} else {
		if (part->parent->world == this) {
			Log::warn("Attempting to readd part to world");
			return;
		}
		TreeNode newNode(part->parent->mainPart, part->parent->mainPart->getStrictBounds(), true);
		for (AttachedPart& p : part->parent->parts) {
			newNode.addInside(TreeNode(p.part, p.part->getStrictBounds(), false));
		}
		objectTree.add(std::move(newNode));
		physicals.add(part->parent);
	}
	part->parent->world = this;
	part->parent->setAnchored(anchored);

	ASSERT_VALID;
}
void WorldPrototype::removePartUnsafe(Part* part) {
	ASSERT_VALID;
	Physical* parent = part->parent;
	parent->detachPart(part);
	
	ASSERT_VALID;
}
void WorldPrototype::removePhysicalUnsafe(Physical* p) {
	NodeStack stack = objectTree.findGroupFor(p->mainPart, p->mainPart->getStrictBounds());
	stack.remove();
	physicals.remove(p);
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
		std::function<void(WorldPrototype&)>& operation = waitingOperations.front();
		operation(*this);
		waitingOperations.pop();
	}
}


void WorldPrototype::addPart(Part* part, bool anchored) {
	requestModification([part, anchored](WorldPrototype& world) {world.addPartUnsafe(part, anchored); });
}
void WorldPrototype::removePart(Part* part) {
	requestModification([part](WorldPrototype& world) {world.removePartUnsafe(part); });
}
void WorldPrototype::removePhysical(Physical* phys) {
	requestModification([phys](WorldPrototype& world) {world.removePhysicalUnsafe(phys); });
}
void WorldPrototype::attachPart(Part* part, Physical& phys, CFrame attachment) {
	requestModification([part, &phys, attachment](WorldPrototype& world) {world.attachPartUnsafe(part, phys, attachment); });
}
void WorldPrototype::detachPart(Part* part) {
	requestModification([part](WorldPrototype& world) {world.detachPartUnsafe(part); });
}

void WorldPrototype::addTerrainPart(Part* part) {
	terrainTree.add(part, part->getStrictBounds());
}
void WorldPrototype::removeTerrainPart(Part* part) {
	terrainTree.remove(part, part->getStrictBounds());
}
void WorldPrototype::optimizeTerrain() {
	for(int i = 0; i < 5; i++)
		terrainTree.improveStructure();
}

void WorldPrototype::setPartCFrame(Part* part, const GlobalCFrame& newCFrame) {
	Bounds oldBounds = part->getStrictBounds();

	part->parent->setPartCFrame(part, newCFrame);

	objectTree.updateObjectGroupBounds(part, oldBounds);
}

void WorldPrototype::updatePartBounds(const Part* updatedPart, const Bounds& oldBounds) {
	objectTree.updateObjectBounds(updatedPart, oldBounds);
}

void WorldPrototype::updatePartGroupBounds(const Part* mainPart, const Bounds& oldMainPartBounds) {
	objectTree.updateObjectGroupBounds(mainPart, oldMainPartBounds);
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
		if (phys.world != this) {
			Log::error("physicals's world is not correct!");
			__debugbreak();
			return false;
		}

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
