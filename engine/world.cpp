#include "world.h"

#include <algorithm>

#ifdef CHECK_SANITY
#define ASSERT_VALID if (!isValid()) {throw "World not valid!";}
#else
#define ASSERT_VALID
#endif


WorldPrototype::WorldPrototype(double deltaT) : deltaT(deltaT) {}

BoundsTree<Part>& WorldPrototype::getTreeForType(PartClass pc) {
	switch (pc) {
	case PartClass::FREE:
		return this->objectTree;
	case PartClass::TERRAIN:
		return this->terrainTree;
	default:
		throw "invalid PartClass!";
	}
}

const BoundsTree<Part>& WorldPrototype::getTreeForType(PartClass pc) const {
	switch (pc) {
	case PartClass::FREE:
		return this->objectTree;
	case PartClass::TERRAIN:
		return this->terrainTree;
	default:
		throw "invalid PartClass!";
	}
}

void WorldPrototype::addPart(Part* part, bool anchored) {
	if (part->parent == nullptr) {
		part->parent = new Physical(part);
		physicals.push_back(part->parent);
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
		physicals.push_back(part->parent);
	}
	part->parent->world = this;
	part->parent->setAnchored(anchored);
	part->partClass = PartClass::FREE;

	ASSERT_VALID;
}
void WorldPrototype::removePart(Part* part) {
	ASSERT_VALID;
	Physical* parent = part->parent;
	parent->detachPart(part);

	ASSERT_VALID;
}
void WorldPrototype::removePhysical(Physical* phys) {
	NodeStack stack = objectTree.findGroupFor(phys->mainPart, phys->mainPart->getStrictBounds());
	stack.remove();
	physicals.erase(std::remove(physicals.begin(), physicals.end(), phys));
	ASSERT_VALID;
}
void WorldPrototype::addTerrainPart(Part* part) {
	terrainTree.add(part, part->getStrictBounds());
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
