#include "world.h"

#include <algorithm>
#include "../util/log.h"
#include "layer.h"
#include "misc/validityHelper.h"

//using namespace P3D::OldBoundsTree;
//using namespace P3D::NewBoundsTree;

// #define CHECK_WORLD_VALIDITY

#ifdef CHECK_WORLD_VALIDITY
#define ASSERT_VALID if (!isValid()) throw "World not valid!";
#else
#define ASSERT_VALID
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

		bool result = true;
		phys->forEachPart([&result](const Part& part) {
			if(part.layer == nullptr) {
				Log::error("Part in physical has no layer!");
				DEBUGBREAK;
				result = false;
			} else {
				if(!part.layer->tree.contains(&part)) {
					Log::error("Part not in tree!");
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
					Log::error("Part contained in layer, but it's layer field is not the layer");
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
	int layerIndex = layers.size();
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



static void createNodeFor(P3D::OldBoundsTree::BoundsTree<Part>& tree, MotorizedPhysical* phys) {
	P3D::OldBoundsTree::TreeNode newNode(phys->rigidBody.mainPart, phys->rigidBody.mainPart->getBounds(), true);
	phys->forEachPartExceptMainPart([&newNode](Part& part) {
		newNode.addInside(P3D::OldBoundsTree::TreeNode(&part, part.getBounds(), false));
	});
	tree.add(std::move(newNode));
}
static void createNodeFor(P3D::NewBoundsTree::BoundsTree<Part>& tree, MotorizedPhysical* phys) {
	if(phys->isSinglePart()) {
		tree.add(phys->getMainPart());
	} else {
		P3D::NewBoundsTree::TrunkAllocator& alloc = tree.getPrototype().getAllocator();
		P3D::NewBoundsTree::TreeTrunk* newNode = alloc.allocTrunk();
		int newNodeSize = 0;

		phys->forEachPart([&alloc, &newNode, &newNodeSize, &tree](Part& p) {
			newNodeSize = P3D::NewBoundsTree::addRecursive(alloc, *newNode, newNodeSize, P3D::NewBoundsTree::TreeNodeRef(static_cast<void*>(&p)), p.getBounds());
		});
		tree.getPrototype().addGroupTrunk(newNode, newNodeSize);
	}
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


	WorldLayer* worldLayer = &layers[layerIndex].subLayers[ColissionLayer::FREE_PARTS_LAYER];
	part->parent->mainPhysical->forEachPart([worldLayer](Part& p) {
		p.layer = worldLayer;
	});
	createNodeFor(worldLayer->tree, part->parent->mainPhysical);


	objectCount += part->parent->mainPhysical->getNumberOfPartsInThisAndChildren();
	
	ASSERT_VALID;

	part->parent->mainPhysical->forEachPart([this](Part& p) {
		this->onPartAdded(&p);
	});

	ASSERT_VALID;
}

#ifdef USE_NEW_BOUNDSTREE
static void createNewNodeFor(MotorizedPhysical* motorPhys, P3D::NewBoundsTree::BoundsTree<Part>& layer, Part* repPart) {
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
		P3D::NewBoundsTree::TrunkAllocator& alloc = layer.getPrototype().getAllocator();
		P3D::NewBoundsTree::TreeTrunk* newNode = alloc.allocTrunk();
		int newNodeSize = 0;

		motorPhys->forEachPart([&alloc, &newNode, &newNodeSize, &layer, repPart](Part& p) {
			if(&p.layer->tree == &layer) {
				newNodeSize = P3D::NewBoundsTree::addRecursive(alloc, *newNode, newNodeSize, P3D::NewBoundsTree::TreeNodeRef(static_cast<void*>(&p)), p.getBounds());
			}
		});
		layer.getPrototype().addGroupTrunk(newNode, newNodeSize);
	}
}
#else
static void createNewNodeFor(MotorizedPhysical* motorPhys, P3D::OldBoundsTree::BoundsTree<Part>& layer, Part* repPart) {
	P3D::OldBoundsTree::TreeNode newNode(repPart, repPart->getBounds(), true);
	motorPhys->forEachPart([&newNode, &layer, repPart](Part& p) {
		if(&p.layer->tree == &layer && &p != repPart) {
			newNode.addInside(P3D::OldBoundsTree::TreeNode(&p, p.getBounds()));
		}
	});
	layer.add(std::move(newNode));
}
#endif

void WorldPrototype::addPhysicalWithExistingLayers(MotorizedPhysical* motorPhys) {
	physicals.push_back(motorPhys);
	motorPhys->world = this;

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
	for(Part& p : this->iterParts()) {
		p.parent = nullptr;
		p.layer = nullptr;
		partsToDelete.push_back(&p);
	}
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

size_t WorldPrototype::getLayerCount() const {
	return this->layers.size();
}

void WorldPrototype::notifyMainPhysicalObsolete(MotorizedPhysical* motorPhys) {
	physicals.erase(std::remove(physicals.begin(), physicals.end(), motorPhys));

	ASSERT_VALID;
}
void WorldPrototype::optimizeLayers() {
	for(ColissionLayer& layer : layers) {
		layer.subLayers[ColissionLayer::TERRAIN_PARTS_LAYER].optimize();
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
