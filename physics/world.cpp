#include "world.h"

#include <algorithm>
#include "../util/log.h"
#include "layer.h"
#include "misc/validityHelper.h"

#ifndef NDEBUG
#define ASSERT_VALID if (!isValid()) throw "World not valid!";
#define ASSERT_TREE_VALID(tree) treeValidCheck(tree)
#else
#define ASSERT_VALID
#define ASSERT_TREE_VALID(tree)
#endif

#pragma region worldValidity

bool WorldPrototype::isValid() const {
	for(const MotorizedPhysical* phys : iterPhysicals()) {
		if(phys->world != this) {
			Log::error("physicals's world is not correct!");
			DEBUGBREAK;
			return false;
		}

		if(!isMotorizedPhysicalValid(phys)) {
			Log::error("Physical invalid!");
			DEBUGBREAK;
			return false;
		}
	}

	for(const WorldLayer& l : layers) {
		treeValidCheck(l.tree);
	}
	return true;
}
#pragma endregion


WorldPrototype::WorldPrototype(double deltaT) : 
	deltaT(deltaT), 
	layers(2),
	colissionMatrix(2),
	objectTree(layers[0].tree), 
	terrainTree(layers[1].tree) {
	colissionMatrix.get(0, 0) = true; // free-free
	colissionMatrix.get(1, 0) = true; // free-terrain
	colissionMatrix.get(1, 1) = false; // terrain-terrain
}

WorldPrototype::~WorldPrototype() {

}

static TreeNode createNodeFor(MotorizedPhysical* phys) {
	TreeNode newNode(phys->rigidBody.mainPart, phys->rigidBody.mainPart->getBounds(), true);
	phys->forEachPartExceptMainPart([&newNode](Part& part) {
		newNode.addInside(TreeNode(&part, part.getBounds(), false));
	});
	return newNode;
}

void WorldPrototype::addPart(Part* part) {
	ASSERT_VALID;
	part->layer = &layers[0];
	part->ensureHasParent();
	if (part->parent->mainPhysical->world == this) {
		Log::warn("Attempting to readd part to world");
		ASSERT_VALID;
		return;
	}
	
	objectTree.add(createNodeFor(part->parent->mainPhysical));
	physicals.push_back(part->parent->mainPhysical);

	objectCount += part->parent->mainPhysical->getNumberOfPartsInThisAndChildren();
	
	part->parent->mainPhysical->world = this;

	ASSERT_VALID;

	part->parent->mainPhysical->forEachPart([this](Part& part) {
		this->onPartAdded(&part);
	});
}
void WorldPrototype::removePart(Part* part) {
	ASSERT_VALID;
	
	if(part->parent == nullptr) {
		this->terrainTree.remove(part);
		this->onPartRemoved(part);
	} else {
		part->parent->removePart(part);
	}

	ASSERT_VALID;
}

void WorldPrototype::clear() {
	this->constraints.clear();
	this->externalForces.clear();
	for(MotorizedPhysical* phys : this->physicals) {
		delete phys;
	}
	this->physicals.clear();
	std::vector<Part*> partsToDelete;
	for(Part& p : this->iterParts(ALL_PARTS)) {
		p.parent = nullptr;
		partsToDelete.push_back(&p);
	}
	this->objectCount = 0;
	for(WorldLayer& layer : this->layers) {
		layer.tree.clear();
	}
	for(Part* p : partsToDelete) {
		this->onPartRemoved(p);
	}
}

void WorldPrototype::notifyMainPhysicalObsolete(MotorizedPhysical* motorPhys) {
	physicals.erase(std::remove(physicals.begin(), physicals.end(), motorPhys));

	ASSERT_VALID;
}
void WorldPrototype::addTerrainPart(Part* part) {
	objectCount++;

	WorldLayer* layer = &layers[1];

	layer->tree.add(part, part->getBounds());
	part->layer = layer;

	ASSERT_VALID;

	this->onPartAdded(part);
}
void WorldPrototype::optimizeTerrain() {
	for(int i = 0; i < 5; i++) {
		terrainTree.improveStructure();
	}
	ASSERT_VALID;
}

void WorldPrototype::notifyNewPhysicalCreatedWhenSplitting(MotorizedPhysical* newPhysical) {
	physicals.push_back(newPhysical);
	newPhysical->world = this;
}

void WorldPrototype::notifyPhysicalHasBeenSplit(const MotorizedPhysical* mainPhysical, MotorizedPhysical* newlySplitPhysical) {
	assert(mainPhysical->world == this);
	assert(newlySplitPhysical->world == nullptr);
	this->notifyNewPhysicalCreatedWhenSplitting(newlySplitPhysical);
	
	ASSERT_TREE_VALID(objectTree);

	// split object tree
	// TODO: The findGroupFor and grap calls can be merged as an optimization
	NodeStack stack = objectTree.findGroupFor(newlySplitPhysical->getMainPart(), newlySplitPhysical->getMainPart()->getBounds());

	TreeNode* node = *stack;

	TreeNode newNode = objectTree.grab(newlySplitPhysical->getMainPart(), newlySplitPhysical->getMainPart()->getBounds());
	if(!newNode.isGroupHead) {
		newNode.isGroupHead = true;

		// node should still be valid at this time

		for(TreeIterator iter(*node); iter != IteratorEnd();) {
			TreeNode* objectNode = *iter;
			Part* part = static_cast<Part*>(objectNode->object);
			if(part->parent->mainPhysical == newlySplitPhysical) {
				newNode.addInside(iter.remove());
			} else {
				++iter;
			}
		}
		stack.updateBoundsAllTheWayToTop();
	}

	objectTree.add(std::move(newNode));

	ASSERT_TREE_VALID(objectTree);
}

static void removePhysicalFromList(std::vector<MotorizedPhysical*>& physicals, MotorizedPhysical* physToRemove) {
	for(MotorizedPhysical*& item : physicals) {
		if(item == physToRemove) {
			item = std::move(physicals.back());
			physicals.pop_back();

			return;
		}
	}
	throw std::logic_error("No physical found to remove!");
}

void WorldPrototype::mergePhysicalGroups(const MotorizedPhysical* firstPhysical, MotorizedPhysical* secondPhysical) {
	assert(firstPhysical->world == this);

	TreeNode newNode;

	if(secondPhysical->world != nullptr) {
		assert(secondPhysical->world == this);
		removePhysicalFromList(this->physicals, secondPhysical);

		newNode = objectTree.grabGroupFor(secondPhysical->getMainPart(), secondPhysical->getMainPart()->getBounds());
	} else {
		secondPhysical->forEachPart([this](Part& part) {
			this->onPartAdded(&part);
		});

		newNode = createNodeFor(secondPhysical);
	}
		
	const Part* main = firstPhysical->getMainPart();
	objectTree.addToExistingGroup(std::move(newNode), main, main->getBounds());

	ASSERT_TREE_VALID(objectTree);
}

void WorldPrototype::notifyPhysicalsMerged(const MotorizedPhysical* firstPhysical, MotorizedPhysical* secondPhysical) {
	mergePhysicalGroups(firstPhysical, secondPhysical);
}

void WorldPrototype::notifyNewPartAddedToPhysical(const MotorizedPhysical* physical, Part* newPart) {
	assert(physical->world == this);

	this->objectTree.addToExistingGroup(newPart, newPart->getBounds(), physical->getMainPart(), physical->getMainPart()->getBounds());
	objectCount++;
	ASSERT_TREE_VALID(objectTree);

	onPartAdded(newPart);
}

void WorldPrototype::notifyPartDetachedFromPhysical(Part* part) {
	assert(part->parent != nullptr);
	assert(part->parent->childPhysicals.size() == 0);
	assert(part->parent->rigidBody.getPartCount() == 1);
	assert(part->parent->isMainPhysical());

	objectTree.moveOutOfGroup(part);
	ASSERT_TREE_VALID(objectTree);
}

void WorldPrototype::notifyPartRemovedFromPhysical(Part* part) {
	assert(part->parent == nullptr);

	objectTree.remove(part);
	objectCount--;
	ASSERT_TREE_VALID(objectTree);

	this->onPartRemoved(part);
}


void WorldPrototype::onPartAdded(Part* newPart) {}
void WorldPrototype::onPartRemoved(Part* removedPart) {}

void WorldPrototype::addExternalForce(ExternalForce* force) {
	externalForces.push_back(force);
}

void WorldPrototype::removeExternalForce(ExternalForce* force) {
	externalForces.erase(std::remove(externalForces.begin(), externalForces.end(), force));
}

IteratorFactoryWithEnd<WorldPartIter> WorldPrototype::iterParts(int partsMask) {
	size_t size = 0;
	IteratorFactoryWithEnd<BoundsTreeIter<TreeIterator, Part>> iters[2]{};
	if(partsMask & FREE_PARTS) {
		BoundsTreeIter<TreeIterator, Part> i(objectTree.begin());
		iters[size++] = IteratorFactoryWithEnd<BoundsTreeIter<TreeIterator, Part>>(i);
	}
	if(partsMask & TERRAIN_PARTS) {
		BoundsTreeIter<TreeIterator, Part> i(terrainTree.begin());
		iters[size++] = IteratorFactoryWithEnd<BoundsTreeIter<TreeIterator, Part>>(i);
	}

	WorldPartIter group(iters, size);

	return IteratorFactoryWithEnd<WorldPartIter>(std::move(group));
}
IteratorFactoryWithEnd<ConstWorldPartIter> WorldPrototype::iterParts(int partsMask) const {
	size_t size = 0;
	IteratorFactoryWithEnd<BoundsTreeIter<ConstTreeIterator, const Part>> iters[2]{};
	if(partsMask & FREE_PARTS) {
		BoundsTreeIter<ConstTreeIterator, const Part> i(objectTree.begin());
		iters[size++] = IteratorFactoryWithEnd<BoundsTreeIter<ConstTreeIterator, const Part>>(i);
	}
	if(partsMask & TERRAIN_PARTS) {
		BoundsTreeIter<ConstTreeIterator, const Part> i(terrainTree.begin());
		iters[size++] = IteratorFactoryWithEnd<BoundsTreeIter<ConstTreeIterator, const Part>>(i);
	}

	ConstWorldPartIter group(iters, size);

	return IteratorFactoryWithEnd<ConstWorldPartIter>(std::move(group));
}



