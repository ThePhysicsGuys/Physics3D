#include "world.h"

#include <algorithm>
#include "misc/debug.h"
#include "layer.h"
#include "misc/validityHelper.h"
#include "worldIteration.h"
#include "threading/threadPool.h"

namespace P3D {
// #define CHECK_WORLD_VALIDITY
#ifdef CHECK_WORLD_VALIDITY
#define ASSERT_VALID if (!isValid()) throw "World not valid!";
#else
#define ASSERT_VALID
#endif

#pragma region worldValidity

bool WorldPrototype::isValid() const {
	for(const MotorizedPhysical* phys : this->physicals) {
		if(phys->getWorld() != this) {
			Debug::logError("physicals's world is not correct!");
			DEBUGBREAK;
			return false;
		}

		if(!isMotorizedPhysicalValid(phys)) {
			Debug::logError("Physical invalid!");
			DEBUGBREAK;
			return false;
		}

		bool result = true;
		phys->forEachPart([&result](const Part& part) {
			if(part.layer == nullptr) {
				Debug::logError("Part in physical has no layer!");
				DEBUGBREAK;
				result = false;
			} else {
				if(!part.layer->tree.contains(&part)) {
					Debug::logError("Part not in tree!");
					DEBUGBREAK;
					result = false;
				}
			}
		});
		if(result == false) return false;
	}

	for(const ColissionLayer& cl : layers) {
		for(const WorldLayer& l : cl.subLayers) {
			treeValidCheck(l.tree);
			for(const Part& p : l.tree) {
				if(p.layer != &l) {
					Debug::logError("Part contained in layer, but it's layer field is not the layer");
					DEBUGBREAK;
					return false;
				}
			}
		}
	}
	return true;
}
#pragma endregion

WorldPrototype::WorldPrototype(double deltaT) :
	deltaT(deltaT),
	layers(),
	colissionMask() {

	layers.emplace_back(this, true);
}

WorldPrototype::~WorldPrototype() {

}

static std::pair<int, int> pairLayers(int layer1, int layer2) {
	if(layer1 < layer2) {
		return std::make_pair(layer1, layer2);
	} else {
		return std::make_pair(layer2, layer1);
	}
}

bool WorldPrototype::doLayersCollide(int layer1, int layer2) const {
	if(layer1 == layer2) {
		return layers[layer1].collidesInternally;
	} else {
		std::pair<int, int> checkPair = pairLayers(layer1, layer2);
		for(std::pair<int, int> itemInColissionMask : colissionMask) {
			if(itemInColissionMask == checkPair) {
				return true;
			}
		}
		return false;
	}
}
void WorldPrototype::setLayersCollide(int layer1, int layer2, bool collide) {
	if(layer1 == layer2) {
		layers[layer1].collidesInternally = collide;
	} else {
		std::pair<int, int> checkAddPair = pairLayers(layer1, layer2);
		for(std::pair<int, int>& itemInColissionMask : colissionMask) {
			if(itemInColissionMask == checkAddPair) {
				if(!collide) {
					// remove for no collide
					itemInColissionMask = colissionMask.back();
					colissionMask.pop_back();
				}
				return;
			}
		}
		if(collide) {
			colissionMask.push_back(checkAddPair);
		}
	}
}

int WorldPrototype::createLayer(bool collidesInternally, bool collidesWithOthers) {
	int layerIndex = static_cast<int>(layers.size());
	layers.emplace_back(this, collidesInternally);
	if(collidesWithOthers) {
		for(int i = 0; i < layerIndex; i++) {
			colissionMask.emplace_back(i, layerIndex);
		}
	}
	return layerIndex;
}
void WorldPrototype::deleteLayer(int layerIndex, int layerToMoveTo) {

}

static void createNodeFor(P3D::BoundsTree<Part>& tree, MotorizedPhysical* phys) {
	if(phys->isSinglePart()) {
		tree.add(phys->getMainPart());
	} else {
		P3D::TrunkAllocator& alloc = tree.getPrototype().getAllocator();
		P3D::TreeTrunk* newNode = alloc.allocTrunk();
		int newNodeSize = 0;

		phys->forEachPart([&alloc, &newNode, &newNodeSize, &tree](Part& p) {
			newNodeSize = P3D::addRecursive(alloc, *newNode, newNodeSize, P3D::TreeNodeRef(static_cast<void*>(&p)), p.getBounds());
		});
		tree.getPrototype().addGroupTrunk(newNode, newNodeSize);
	}
}

void WorldPrototype::addPart(Part* part, int layerIndex) {
	ASSERT_VALID;

	if(part->layer) {
		Debug::logWarn("This part is already in a world");
		ASSERT_VALID;
		return;
	}

	Physical* partPhys = part->ensureHasPhysical();
	physicals.push_back(partPhys->mainPhysical);

	WorldLayer* worldLayer = &layers[layerIndex].subLayers[ColissionLayer::FREE_PARTS_LAYER];
	partPhys->mainPhysical->forEachPart([worldLayer](Part& p) {
		p.layer = worldLayer;
	});
	createNodeFor(worldLayer->tree, partPhys->mainPhysical);


	objectCount += partPhys->mainPhysical->getNumberOfPartsInThisAndChildren();

	ASSERT_VALID;

	partPhys->mainPhysical->forEachPart([this](Part& p) {
		this->onPartAdded(&p);
	});

	ASSERT_VALID;
}

static void createNewNodeFor(MotorizedPhysical* motorPhys, BoundsTree<Part>& layer, Part* repPart) {
	size_t totalParts = 0;
	motorPhys->forEachPart([&layer, &totalParts](Part& p) {
		if(&p.layer->tree == &layer) {
			totalParts++;
		}
	});

	assert(totalParts >= 1);
	if(totalParts == 1) {
		layer.add(repPart);
	} else {
		TrunkAllocator& alloc = layer.getPrototype().getAllocator();
		TreeTrunk* newNode = alloc.allocTrunk();
		int newNodeSize = 0;

		motorPhys->forEachPart([&alloc, &newNode, &newNodeSize, &layer, repPart](Part& p) {
			if(&p.layer->tree == &layer) {
				newNodeSize = addRecursive(alloc, *newNode, newNodeSize, TreeNodeRef(static_cast<void*>(&p)), p.getBounds());
			}
		});
		layer.getPrototype().addGroupTrunk(newNode, newNodeSize);
	}
}

void WorldPrototype::addPhysicalWithExistingLayers(MotorizedPhysical* motorPhys) {
	physicals.push_back(motorPhys);

	std::vector<FoundLayerRepresentative> foundLayers = findAllLayersIn(motorPhys);

	for(const FoundLayerRepresentative& l : foundLayers) {
		createNewNodeFor(motorPhys, l.layer->tree, l.part);
	}

	ASSERT_VALID;
}

void WorldPrototype::addTerrainPart(Part* part, int layerIndex) {
	objectCount++;

	WorldLayer* worldLayer = &layers[layerIndex].subLayers[ColissionLayer::TERRAIN_PARTS_LAYER];
	part->layer = worldLayer;
	worldLayer->addPart(part);

	ASSERT_VALID;

	this->onPartAdded(part);

	ASSERT_VALID;
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
	this->forEachPart([&partsToDelete](Part& part) {
		part.parent = nullptr;
		part.layer = nullptr;
		partsToDelete.push_back(&part);
	});
	this->objectCount = 0;
	for(ColissionLayer& cl : this->layers) {
		for(WorldLayer& layer : cl.subLayers) {
			layer.tree.clear();
		}
	}
	for(Part* p : partsToDelete) {
		this->onPartRemoved(p);
		this->deletePart(p);
	}
}

int WorldPrototype::getLayerCount() const {
	return static_cast<int>(this->layers.size());
}

void WorldPrototype::optimizeLayers() {
	for(ColissionLayer& layer : layers) {
		layer.subLayers[ColissionLayer::TERRAIN_PARTS_LAYER].optimize();
	}
	ASSERT_VALID;
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
	assert(mainPhysical->getWorld() == this);
	assert(newlySplitPhysical->getWorld() == nullptr);
	this->physicals.push_back(newlySplitPhysical);

	std::vector<std::pair<WorldLayer*, std::vector<const Part*>>> layersThatNeedToBeSplit;
	assignLayersForPhysicalRecurse(*newlySplitPhysical, layersThatNeedToBeSplit);

	for(const std::pair<WorldLayer*, std::vector<const Part*>>& layer : layersThatNeedToBeSplit) {
		layer.first->splitGroup(layer.second.begin(), layer.second.end());
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

void WorldPrototype::onPartAdded(Part* newPart) {}
void WorldPrototype::onPartRemoved(Part* removedPart) {}

void WorldPrototype::addExternalForce(ExternalForce* force) {
	externalForces.push_back(force);
}

void WorldPrototype::removeExternalForce(ExternalForce* force) {
	externalForces.erase(std::remove(externalForces.begin(), externalForces.end(), force));
}
};