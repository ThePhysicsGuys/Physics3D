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

static std::vector<WorldLayer> produceLayerVector(WorldPrototype* world, size_t count) {
	std::vector<WorldLayer> result;
	result.reserve(count);

	for(size_t i = 0; i < count; i++) {
		result.emplace_back(world);
	}

	return result;
}

WorldPrototype::WorldPrototype(double deltaT) : 
	deltaT(deltaT), 
	layers(produceLayerVector(this, 2)),
	freePartColissions(),
	freeTerrainColissions{std::make_pair(&layers[0], &layers[1])}, // free-terrain
	internalColissions{&layers[0]}, // free-free
	objectTree(layers[0].tree), 
	terrainTree(layers[1].tree),
	layersToRefresh{&layers[0]} {

}

WorldPrototype::~WorldPrototype() {

}

void WorldPrototype::addLink(SoftLink* softLink) {
	softLinks.push_back(softLink);
}

static TreeNode createNodeFor(MotorizedPhysical* phys) {
	TreeNode newNode(phys->rigidBody.mainPart, phys->rigidBody.mainPart->getBounds(), true);
	phys->forEachPartExceptMainPart([&newNode](Part& part) {
		newNode.addInside(TreeNode(&part, part.getBounds(), false));
	});
	return newNode;
}

void WorldPrototype::addPart(Part* part, int layerIndex) {
	ASSERT_VALID;
	
	if(part->layer) {
		Log::warn("This part is already in a world");
		ASSERT_VALID;
		return;
	}

	part->ensureHasParent();
	physicals.push_back(part->parent->mainPhysical);
	part->parent->mainPhysical->world = this;

	WorldLayer* worldLayer = &layers[layerIndex];
	part->parent->mainPhysical->forEachPart([worldLayer](Part& p) {
		p.layer = worldLayer;
	});
	worldLayer->tree.add(createNodeFor(part->parent->mainPhysical));


	objectCount += part->parent->mainPhysical->getNumberOfPartsInThisAndChildren();
	
	ASSERT_VALID;

	part->parent->mainPhysical->forEachPart([this](Part& p) {
		this->onPartAdded(&p);
	});

	ASSERT_VALID;
}
void WorldPrototype::addTerrainPart(Part* part, int layerIndex) {
	objectCount++;

	WorldLayer* worldLayer = &layers[layerIndex];
	part->layer = worldLayer;
	worldLayer->addPart(part);

	ASSERT_VALID;

	this->onPartAdded(part);
}
void WorldPrototype::removePart(Part* part) {
	ASSERT_VALID;
	
	part->removeFromWorld();

	ASSERT_VALID;
}

void WorldPrototype::deletePart(Part* partToDelete) const {
	delete partToDelete;
}

void WorldPrototype::clear() {
	this->constraints.clear();
	this->externalForces.clear();
	for(MotorizedPhysical* phys : this->physicals) {
		delete phys;
	}
	this->physicals.clear();
	std::vector<Part*> partsToDelete;
	for(Part& p : this->iterParts()) {
		p.parent = nullptr;
		p.layer = nullptr;
		partsToDelete.push_back(&p);
	}
	this->objectCount = 0;
	for(WorldLayer& layer : this->layers) {
		layer.tree.clear();
	}
	for(Part* p : partsToDelete) {
		this->onPartRemoved(p);
		this->deletePart(p);
	}
}

size_t WorldPrototype::getLayerCount() const {
	return this->layers.size();
}

BoundsTree<Part>& WorldPrototype::getTree(int index) {
	return layers[index].tree;
}

const BoundsTree<Part>& WorldPrototype::getTree(int index) const {
	return layers[index].tree;
}

void WorldPrototype::notifyMainPhysicalObsolete(MotorizedPhysical* motorPhys) {
	physicals.erase(std::remove(physicals.begin(), physicals.end(), motorPhys));

	ASSERT_VALID;
}
void WorldPrototype::optimizeLayers() {
	for(WorldLayer& layer : layers) {
		layer.optimize();
	}
	ASSERT_VALID;
}

void WorldPrototype::notifyNewPhysicalCreated(MotorizedPhysical* newPhysical) {
	physicals.push_back(newPhysical);
	newPhysical->world = this;
}

static void assignLayersForPhysicalRecurse(const Physical& phys, std::vector<std::pair<WorldLayer*, std::vector<const Part*>>>& foundLayers) {
	phys.rigidBody.forEachPart([&foundLayers](const Part& part) {
		for(std::pair<WorldLayer*, std::vector<const Part*>>& knownLayer : foundLayers) {
			if(part.layer == knownLayer.first) {
				knownLayer.second.push_back(&part);
				return;
			}
		}
		foundLayers.emplace_back(part.layer, std::vector<const Part*>{&part});
	});
	for(const ConnectedPhysical& conPhys : phys.childPhysicals) {
		assignLayersForPhysicalRecurse(conPhys, foundLayers);
	}
}

void WorldPrototype::notifyPhysicalHasBeenSplit(const MotorizedPhysical* mainPhysical, MotorizedPhysical* newlySplitPhysical) {
	assert(mainPhysical->world == this);
	assert(newlySplitPhysical->world == nullptr);
	this->notifyNewPhysicalCreated(newlySplitPhysical);
	
	std::vector<std::pair<WorldLayer*, std::vector<const Part*>>> layersThatNeedToBeSplit;
	assignLayersForPhysicalRecurse(*newlySplitPhysical, layersThatNeedToBeSplit);

	for(const std::pair<WorldLayer*, std::vector<const Part*>>& layer : layersThatNeedToBeSplit) {
		layer.first->moveAllOutOfGroup(layer.second.begin(), layer.second.end());
	}


	// split object tree
	// TODO: The findGroupFor and grap calls can be merged as an optimization
	/*NodeStack stack = objectTree.findGroupFor(newlySplitPhysical->getMainPart(), newlySplitPhysical->getMainPart()->getBounds());

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

	assert(this->isValid());*/


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

void WorldPrototype::notifyPhysicalsMerged(const MotorizedPhysical* firstPhysical, MotorizedPhysical* secondPhysical) {
	assert(firstPhysical->world == this);

	if(secondPhysical->world != nullptr) {
		assert(secondPhysical->world == this);
		removePhysicalFromList(this->physicals, secondPhysical);
	}
}

void WorldPrototype::notifyNewPartAddedToPhysical(const MotorizedPhysical* physical, Part* newPart) {
	assert(physical->world == this);

	/*this->objectTree.addToExistingGroup(newPart, newPart->getBounds(), physical->getMainPart(), physical->getMainPart()->getBounds());
	objectCount++;
	ASSERT_TREE_VALID(objectTree);*/

	onPartAdded(newPart);
}

void WorldPrototype::onPartAdded(Part* newPart) {}
void WorldPrototype::onPartRemoved(Part* removedPart) {}

void WorldPrototype::addExternalForce(ExternalForce* force) {
	externalForces.push_back(force);
}

void WorldPrototype::removeExternalForce(ExternalForce* force) {
	externalForces.erase(std::remove(externalForces.begin(), externalForces.end(), force));
}
