#include "world.h"

#include <algorithm>

#ifndef NDEBUG
#define ASSERT_VALID if (!isValid()) {throw "World not valid!";}
#else
#define ASSERT_VALID
#endif


WorldPrototype::WorldPrototype(double deltaT) : deltaT(deltaT) {}

BoundsTree<Part>& WorldPrototype::getTreeForPart(const Part* part) {
	return (part->isTerrainPart) ? this->terrainTree : this->objectTree;
}

const BoundsTree<Part>& WorldPrototype::getTreeForPart(const Part* part) const {
	return (part->isTerrainPart) ? this->terrainTree : this->objectTree;
}

void WorldPrototype::addPart(Part* part, bool anchored) {
	ASSERT_VALID;
	if (part->parent == nullptr) {
		part->parent = new Physical(part);
		physicals.push_back(part->parent);
		objectTree.add(part, part->getStrictBounds());

		objectCount++;
	} else {
		if (part->parent->world == this) {
			Log::warn("Attempting to readd part to world");
			ASSERT_VALID;
			return;
		}
		TreeNode newNode(part->parent->mainPart, part->parent->mainPart->getStrictBounds(), true);
		for (AttachedPart& p : part->parent->parts) {
			newNode.addInside(TreeNode(p.part, p.part->getStrictBounds(), false));
		}
		objectTree.add(std::move(newNode));
		physicals.push_back(part->parent);

		objectCount += part->parent->getPartCount();
	}
	part->parent->world = this;
	part->parent->setAnchored(anchored);

	ASSERT_VALID;
}
void WorldPrototype::removePart(Part* part) {
	ASSERT_VALID;
	Physical* parent = part->parent;
	parent->detachPart(part);

	objectCount--;

	ASSERT_VALID;
}
void WorldPrototype::removePhysical(Physical* phys) {
	objectCount -= phys->getPartCount();

	NodeStack stack = objectTree.findGroupFor(phys->mainPart, phys->mainPart->getStrictBounds());
	stack.remove();
	physicals.erase(std::remove(physicals.begin(), physicals.end(), phys));

	ASSERT_VALID;
}
void WorldPrototype::addTerrainPart(Part* part) {
	objectCount++;

	terrainTree.add(part, part->getStrictBounds());
	part->isTerrainPart = true;
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

void WorldPrototype::removePartFromTrees(const Part* part) {
	getTreeForPart(part).remove(part);
}



void WorldPrototype::addExternalForce(ExternalForce* force) {
	externalForces.push_back(force);
}

void WorldPrototype::removeExternalForce(ExternalForce* force) {
	externalForces.erase(std::remove(externalForces.begin(), externalForces.end(), force));
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
