#include "boundsTree2.h"


#include "aligned_alloc.h"

namespace P3D::NewBoundsTree {

BoundsTemplate<float> TreeTrunk::getBoundsOfSubNode(int subNode) const {
	assert(subNode >= 0 && subNode < BRANCH_FACTOR);
	BoundsTemplate<float> result;
	result.min.x = xMin[subNode];
	result.min.y = yMin[subNode];
	result.min.z = zMin[subNode];
	result.max.x = xMax[subNode];
	result.max.y = yMax[subNode];
	result.max.z = zMax[subNode];
	return result;
}

void TreeTrunk::setBoundsOfSubNode(int subNode, const BoundsTemplate<float>& newBounds) {
	assert(subNode >= 0 && subNode < BRANCH_FACTOR);
	xMin[subNode] = newBounds.min.x;
	yMin[subNode] = newBounds.min.y;
	zMin[subNode] = newBounds.min.z;
	xMax[subNode] = newBounds.max.x;
	yMax[subNode] = newBounds.max.y;
	zMax[subNode] = newBounds.max.z;
}

void TreeTrunk::setSubNode(int subNode, TreeNodeRef&& newNode, const BoundsTemplate<float>& newBounds) {
	assert(subNode >= 0 && subNode < BRANCH_FACTOR);
	subNodes[subNode] = std::move(newNode);
	setBoundsOfSubNode(subNode, newBounds);
}

// naive implementation, to be optimized
BoundsTemplate<float> TrunkSIMDHelperFallback::getTotalBounds(const TreeTrunk& trunk, int upTo) {
	assert(upTo > 1 && upTo <= BRANCH_FACTOR);
	BoundsTemplate<float> totalBounds = trunk.getBoundsOfSubNode(0);
	for(int i = 1; i < upTo; i++) {
		totalBounds = unionOfBounds(totalBounds, trunk.getBoundsOfSubNode(i));
	}
	return totalBounds;
}

std::array<bool, BRANCH_FACTOR> TrunkSIMDHelperFallback::getAllContainsBounds(const TreeTrunk& trunk, const BoundsTemplate<float>& boundsToContain) {
	std::array<bool, BRANCH_FACTOR> contained;
	for(int i = 0; i < BRANCH_FACTOR; i++) {
		BoundsTemplate<float> subNodeBounds = trunk.getBoundsOfSubNode(i);
		contained[i] = subNodeBounds.contains(boundsToContain);
	}
	return contained;
}

std::array<float, BRANCH_FACTOR> TrunkSIMDHelperFallback::computeAllCombinationCosts(const TreeTrunk& trunk, const BoundsTemplate<float>& boundsExtention) {
	std::array<float, BRANCH_FACTOR> costs;
	for(int i = 0; i < BRANCH_FACTOR; i++) {
		BoundsTemplate<float> subNodeBounds = trunk.getBoundsOfSubNode(i);
		costs[i] = computeCost(unionOfBounds(boundsExtention, subNodeBounds));
	}
	return costs;
}

int TrunkSIMDHelperFallback::getLowestCombinationCost(const TreeTrunk& trunk, const BoundsTemplate<float>& boundsExtention, int nodeSize) {
	std::array<float, BRANCH_FACTOR> costs = TrunkSIMDHelperFallback::computeAllCombinationCosts(trunk, boundsExtention);
	float bestCost = costs[0];
	int bestIndex = 0;
	for(int i = 1; i < nodeSize; i++) {
		if(costs[i] < bestCost) {
			bestIndex = i;
			bestCost = costs[i];
		}
	}
	return bestIndex;
}

std::array<bool, BRANCH_FACTOR> TrunkSIMDHelperFallback::computeOverlapsWith(const TreeTrunk& trunk, int trunkSize, const BoundsTemplate<float>& bounds) {
	std::array<bool, BRANCH_FACTOR> result;

	for(int i = 0; i < trunkSize; i++) {
		result[i] = intersects(trunk.getBoundsOfSubNode(i), bounds);
	}

	return result;
}

std::array<std::array<bool, BRANCH_FACTOR>, BRANCH_FACTOR> TrunkSIMDHelperFallback::computeBoundsOverlapMatrix(const TreeTrunk& trunkA, int trunkASize, const TreeTrunk& trunkB, int trunkBSize) {
	std::array<std::array<bool, BRANCH_FACTOR>, BRANCH_FACTOR> result;
	for(int a = 0; a < trunkASize; a++) {
		BoundsTemplate<float> aBounds = trunkA.getBoundsOfSubNode(a);
		for(int b = 0; b < trunkBSize; b++) {
			BoundsTemplate<float> bBounds = trunkB.getBoundsOfSubNode(b);
			result[a][b] = intersects(aBounds, bBounds);
		}
	}
	return result;
}

std::array<std::array<bool, BRANCH_FACTOR>, BRANCH_FACTOR> TrunkSIMDHelperFallback::computeInternalBoundsOverlapMatrix(const TreeTrunk& trunk, int trunkSize) {
	std::array<std::array<bool, BRANCH_FACTOR>, BRANCH_FACTOR> result;

	for(int a = 0; a < trunkSize; a++) {
		BoundsTemplate<float> aBounds = trunk.getBoundsOfSubNode(a);
		for(int b = a+1; b < trunkSize; b++) {
			BoundsTemplate<float> bBounds = trunk.getBoundsOfSubNode(b);
			result[a][b] = intersects(aBounds, bBounds);
		}
	}

	return result;
}

void TreeTrunk::moveSubNode(int from, int to) {
	this->setSubNode(to, std::move(this->subNodes[from]), this->getBoundsOfSubNode(from));
}

// returns the new size of this node, to be applied to the caller
int addRecursive(TrunkAllocator& allocator, TreeTrunk& curTrunk, int curTrunkSize, TreeNodeRef&& newNode, const BoundsTemplate<float>& bounds) {
	assert(curTrunkSize >= 0 && curTrunkSize <= BRANCH_FACTOR);
	if(curTrunkSize == BRANCH_FACTOR) {
		int chosenNode = TrunkSIMDHelperFallback::getLowestCombinationCost(curTrunk, bounds, curTrunkSize);

		TreeNodeRef& chosen = curTrunk.subNodes[chosenNode];
		BoundsTemplate<float> oldSubNodeBounds = curTrunk.getBoundsOfSubNode(chosenNode);

		// can be inserted into the trunkNode
		if(chosen.isTrunkNode() && !chosen.isGroupHead()) {
			int newSize = addRecursive(allocator, chosen.asTrunk(), chosen.getTrunkSize(), std::move(newNode), bounds);
			chosen.setTrunkSize(newSize);
		} else {
			TreeTrunk* newTrunk = allocator.allocTrunk();
			newTrunk->setSubNode(0, std::move(chosen), oldSubNodeBounds);
			newTrunk->setSubNode(1, std::move(newNode), bounds);

			chosen = TreeNodeRef(newTrunk, 2, false);
		}
		curTrunk.setBoundsOfSubNode(chosenNode, unionOfBounds(oldSubNodeBounds, bounds));
		return BRANCH_FACTOR;
	} else {
		curTrunk.setSubNode(curTrunkSize, std::move(newNode), bounds);
		return curTrunkSize + 1;
	}
}

bool containsObjectRecursive(const TreeTrunk& trunk, int trunkSize, const void* object, const BoundsTemplate<float>& bounds) {
	assert(trunkSize >= 0 && trunkSize <= BRANCH_FACTOR);
	std::array<bool, BRANCH_FACTOR> couldContain = TrunkSIMDHelperFallback::getAllContainsBounds(trunk, bounds);
	for(int i = 0; i < trunkSize; i++) {
		if(!couldContain[i]) continue; 

		const TreeNodeRef& subNode = trunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			if(containsObjectRecursive(subNode.asTrunk(), subNode.getTrunkSize(), object, bounds)) {
				return true;
			}
		} else {
			if(subNode.asObject() == object) {
				return true;
			}
		}
	}
	return false;
}

// returns new trunkSize if removed, -1 if not removed
int removeRecursive(TrunkAllocator& allocator, TreeTrunk& curTrunk, int curTrunkSize, const void* objectToRemove, const BoundsTemplate<float>& bounds) {
	assert(curTrunkSize >= 0 && curTrunkSize <= BRANCH_FACTOR);
	std::array<bool, BRANCH_FACTOR> couldContain = TrunkSIMDHelperFallback::getAllContainsBounds(curTrunk, bounds);
	for(int i = 0; i < curTrunkSize; i++) {
		if(!couldContain[i]) continue;

		TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			TreeTrunk& subNodeTrunk = subNode.asTrunk();
			assert(subNode.getTrunkSize() >= 2);
			int newSubNodeTrunkSize = removeRecursive(allocator, subNodeTrunk, subNode.getTrunkSize(), objectToRemove, bounds);
			assert(newSubNodeTrunkSize >= 1 || newSubNodeTrunkSize == -1);
			if(newSubNodeTrunkSize != -1) {
				if(newSubNodeTrunkSize == 1) {
					// remove reduntant trunk
					bool wasGroupHead = subNode.isGroupHead();
					subNode = std::move(subNodeTrunk.subNodes[0]);
					curTrunk.setBoundsOfSubNode(i, subNodeTrunk.getBoundsOfSubNode(0));
					if(wasGroupHead && subNode.isTrunkNode()) {
						subNode.makeGroupHead(); // in the rare case that it removes Z in a node that is a group like this: GH[TN[X,Y],Z]. Without this the grouping would be lost
					}
					allocator.freeTrunk(&subNodeTrunk);
				} else {
					curTrunk.setBoundsOfSubNode(i, TrunkSIMDHelperFallback::getTotalBounds(subNodeTrunk, newSubNodeTrunkSize));
					subNode.setTrunkSize(newSubNodeTrunkSize);
				}
					
				return curTrunkSize;
			}
		} else {
			if(subNode.asObject() == objectToRemove) {
				curTrunk.moveSubNode(curTrunkSize - 1, i);
				return curTrunkSize - 1;
			}
		}
	}
	return -1;
}

struct TreeGrab {
	int resultingGroupSize;
	TreeNodeRef nodeRef;
	BoundsTemplate<float> nodeBounds;

	TreeGrab() : resultingGroupSize(-1) {}
	TreeGrab(int resultingGroupSize, TreeNodeRef&& nodeRef, const BoundsTemplate<float>& nodeBounds) :
		resultingGroupSize(resultingGroupSize),
		nodeRef(std::move(nodeRef)),
		nodeBounds(nodeBounds) {}
};

// returns new trunkSize if removed, -1 if not removed
static TreeGrab grabGroupRecursive(TrunkAllocator& allocator, TreeTrunk& curTrunk, int curTrunkSize, const void* groupRepresentative, const BoundsTemplate<float>& representativeBounds) {
	assert(curTrunkSize >= 0 && curTrunkSize <= BRANCH_FACTOR);
	std::array<bool, BRANCH_FACTOR> couldContain = TrunkSIMDHelperFallback::getAllContainsBounds(curTrunk, representativeBounds);
	for(int i = 0; i < curTrunkSize; i++) {
		if(!couldContain[i]) continue;

		TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			TreeTrunk& subNodeTrunk = subNode.asTrunk();
			size_t trunkSize = subNode.getTrunkSize();
			assert(trunkSize >= 2);

			if(subNode.isGroupHead()) {
				if(containsObjectRecursive(subNodeTrunk, trunkSize, groupRepresentative, representativeBounds)) {
					// found group, now remove it
					TreeNodeRef subNodeCopy = std::move(subNode);
					curTrunk.moveSubNode(curTrunkSize - 1, i);
					return TreeGrab(curTrunkSize - 1, std::move(subNodeCopy), curTrunk.getBoundsOfSubNode(i));
				}
			} else {
				// try 
				TreeGrab recursiveResult = grabGroupRecursive(allocator, subNodeTrunk, subNode.getTrunkSize(), groupRepresentative, representativeBounds);
				int newSubNodeTrunkSize = recursiveResult.resultingGroupSize;
				assert(newSubNodeTrunkSize >= 1 || newSubNodeTrunkSize == -1);
				if(newSubNodeTrunkSize != -1) {
					if(newSubNodeTrunkSize == 1) {
						// remove reduntant trunk
						curTrunk.setSubNode(i, std::move(subNodeTrunk.subNodes[0]), subNodeTrunk.getBoundsOfSubNode(0));
						allocator.freeTrunk(&subNodeTrunk);
					} else {
						curTrunk.setBoundsOfSubNode(i, TrunkSIMDHelperFallback::getTotalBounds(subNodeTrunk, newSubNodeTrunkSize));
						subNode.setTrunkSize(newSubNodeTrunkSize);
					}

					recursiveResult.resultingGroupSize = curTrunkSize;
					return recursiveResult;
				}
			}

		} else {
			if(subNode.asObject() == groupRepresentative) {
				TreeNodeRef subNodeCopy = std::move(subNode);
				BoundsTemplate<float> subNodeBounds = curTrunk.getBoundsOfSubNode(i);
				curTrunk.moveSubNode(curTrunkSize - 1, i);
				return TreeGrab(curTrunkSize - 1, std::move(subNodeCopy), subNodeBounds);
			}
		}
	}
	return TreeGrab();
}

const TreeNodeRef* getGroupRecursive(const TreeTrunk& curTrunk, int curTrunkSize, const void* groupRepresentative, const BoundsTemplate<float>& representativeBounds) {
	assert(curTrunkSize >= 0 && curTrunkSize <= BRANCH_FACTOR);
	std::array<bool, BRANCH_FACTOR> couldContain = TrunkSIMDHelperFallback::getAllContainsBounds(curTrunk, representativeBounds);
	for(int i = 0; i < curTrunkSize; i++) {
		if(!couldContain[i]) continue;

		const TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			const TreeTrunk& subNodeTrunk = subNode.asTrunk();
			size_t subTrunkSize = subNode.getTrunkSize();
			assert(subTrunkSize >= 2);

			if(subNode.isGroupHead()) {
				if(containsObjectRecursive(subNodeTrunk, subTrunkSize, groupRepresentative, representativeBounds)) {
					return &subNode;
				}
			} else {
				const TreeNodeRef* recursiveFound = getGroupRecursive(subNodeTrunk, subTrunkSize, groupRepresentative, representativeBounds);
				if(recursiveFound != nullptr) {
					return recursiveFound;
				}
			}
		} else {
			if(subNode.asObject() == groupRepresentative) {
				return &subNode;
			}
		}
	}
	return nullptr;
}

// also frees starting trunk
static void freeTrunksRecursive(TrunkAllocator& alloc, TreeTrunk& curTrunk, int curTrunkSize) {
	for(int i = 0; i < curTrunkSize; i++) {
		TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			TreeTrunk& subTrunk = subNode.asTrunk();
			freeTrunksRecursive(alloc, subTrunk, subNode.getTrunkSize());
		}
	}
	alloc.freeTrunk(&curTrunk);
}

// expects a function of the form void(void* object, const BoundsTemplate<float>& bounds)
template<typename Func>
static void forEachRecurseWithBounds(const TreeTrunk& curTrunk, int curTrunkSize, const Func& func) {
	for(int i = 0; i < curTrunkSize; i++) {
		const TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			forEachRecurseWithBounds(subNode.asTrunk(), subNode.getTrunkSize(), func);
		} else {
			func(subNode.asObject(), curTrunk.getBoundsOfSubNode(i));
		}
	}
}

// returns true if the group that is inserted into is found
// deletes all trunknodes of the destroyed group with the provided allocator
static bool insertGroupIntoGroup(TrunkAllocator& sourceAlloc, TrunkAllocator& destinationAlloc, TreeTrunk& baseTrunk, int baseTrunkSize, const void* groupToAddToRep, const BoundsTemplate<float>& groupToAddToRepBounds, TreeNodeRef&& groupToDestroy, const BoundsTemplate<float>& groupToDestroyBounds) {
	bool groupWasFound = modifyGroupRecursive(destinationAlloc, baseTrunk, baseTrunkSize, groupToAddToRep, groupToAddToRepBounds, [&sourceAlloc, &destinationAlloc, &groupToDestroy, &groupToDestroyBounds](TreeNodeRef& group, const BoundsTemplate<float>& groupBounds) {
		TreeTrunk* trunk;
		int trunkSize;
		if(group.isLeafNode()) {
			trunk = destinationAlloc.allocTrunk();
			trunk->setSubNode(0, std::move(group), groupBounds);
			trunkSize = 1;
		} else {
			trunk = &group.asTrunk();
			trunkSize = group.getTrunkSize();
		}

		if(groupToDestroy.isTrunkNode()) {
			forEachRecurseWithBounds(groupToDestroy.asTrunk(), groupToDestroy.getTrunkSize(), [&](void* object, const BoundsTemplate<float>& bounds) {
				trunkSize = addRecursive(destinationAlloc, *trunk, trunkSize, TreeNodeRef(object), bounds);
			});
		} else {
			trunkSize = addRecursive(destinationAlloc, *trunk, trunkSize, TreeNodeRef(groupToDestroy.asObject()), groupToDestroyBounds);
		}

		group = TreeNodeRef(trunk, trunkSize, true);

		if(groupToDestroy.isTrunkNode()) {
			TreeTrunk& destroyTrunk = groupToDestroy.asTrunk();
			freeTrunksRecursive(sourceAlloc, destroyTrunk, groupToDestroy.getTrunkSize());
		}

		return TrunkSIMDHelperFallback::getTotalBounds(*trunk, trunkSize);
	});
	return groupWasFound;
}



TrunkAllocator::~TrunkAllocator() {
	assert(this->allocationCount == 0);
}
TreeTrunk* TrunkAllocator::allocTrunk() {
	this->allocationCount++;
	return static_cast<TreeTrunk*>(aligned_malloc(sizeof(TreeTrunk), alignof(TreeTrunk)));
}
void TrunkAllocator::freeTrunk(TreeTrunk* trunk) {
	this->allocationCount--;
	aligned_free(trunk);
}
void TrunkAllocator::freeAllTrunks(TreeTrunk& baseTrunk, int baseTrunkSize) {
	for(int i = 0; i < baseTrunkSize; i++) {
		TreeNodeRef& subNode = baseTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			freeTrunksRecursive(*this, subNode.asTrunk(), subNode.getTrunkSize());
		}
	}
}

BoundsTreePrototype::BoundsTreePrototype() : baseTrunk(), baseTrunkSize(0) {}
BoundsTreePrototype::~BoundsTreePrototype() {
	this->clear();
}
void BoundsTreePrototype::addGroupTrunk(TreeTrunk* newTrunk, int newTrunkSize) {
	this->baseTrunkSize = addRecursive(allocator, baseTrunk, baseTrunkSize, TreeNodeRef(newTrunk, newTrunkSize, true), TrunkSIMDHelperFallback::getTotalBounds(*newTrunk, newTrunkSize));
}
void BoundsTreePrototype::add(void* newObject, const BoundsTemplate<float>& bounds) {
	this->baseTrunkSize = addRecursive(allocator, baseTrunk, baseTrunkSize, TreeNodeRef(newObject), bounds);
}
void BoundsTreePrototype::addToGroup(void* newObject, const BoundsTemplate<float>& newObjectBounds, const void* groupRepresentative, const BoundsTemplate<float>& groupRepBounds) {
	bool foundGroup = modifyGroupRecursive(allocator, baseTrunk, baseTrunkSize, groupRepresentative, groupRepBounds, [this, newObject, &newObjectBounds](TreeNodeRef& groupNode, const BoundsTemplate<float>& groupNodeBounds) {
		assert(groupNode.isGroupHeadOrLeaf());
		if(groupNode.isTrunkNode()) {
			TreeTrunk& groupTrunk = groupNode.asTrunk();
			int resultingSize = addRecursive(allocator, groupTrunk, groupNode.getTrunkSize(), TreeNodeRef(newObject), newObjectBounds);
			groupNode.setTrunkSize(resultingSize);
			return TrunkSIMDHelperFallback::getTotalBounds(groupTrunk, resultingSize);
		} else {
			TreeTrunk* newTrunkNode = allocator.allocTrunk();
			newTrunkNode->setSubNode(0, std::move(groupNode), groupNodeBounds);
			newTrunkNode->setSubNode(1, TreeNodeRef(newObject), newObjectBounds);
			groupNode = TreeNodeRef(newTrunkNode, 2, true);
			return TrunkSIMDHelperFallback::getTotalBounds(*newTrunkNode, 2);
		}
	});
	if(!foundGroup) {
		throw "Group not found!";
	}
}
void BoundsTreePrototype::mergeGroups(const void* groupRepA, const BoundsTemplate<float>& repABounds, const void* groupRepB, const BoundsTemplate<float>& repBBounds) {
	TreeGrab grabbed = grabGroupRecursive(this->allocator, this->baseTrunk, this->baseTrunkSize, groupRepA, repABounds);
	if(grabbed.resultingGroupSize == -1) {
		throw "groupRepA not found!";
	}
	this->baseTrunkSize = grabbed.resultingGroupSize;

	bool groupBWasFound = insertGroupIntoGroup(this->allocator, this->allocator, this->baseTrunk, this->baseTrunkSize, groupRepB, repBBounds, std::move(grabbed.nodeRef), grabbed.nodeBounds);

	if(!groupBWasFound) {
		throw "groupRepB not found!";
	}
}

void BoundsTreePrototype::transferGroupTo(const void* groupRep, const BoundsTemplate<float>& groupRepBounds, BoundsTreePrototype& destinationTree) {
	TreeGrab grabbed = grabGroupRecursive(this->allocator, this->baseTrunk, this->baseTrunkSize, groupRep, groupRepBounds);
	if(grabbed.resultingGroupSize == -1) {
		throw "groupRep not found!";
	}
	this->baseTrunkSize = grabbed.resultingGroupSize;

	bool groupBWasFound = addRecursive(destinationTree.allocator, destinationTree.baseTrunk, destinationTree.baseTrunkSize, std::move(grabbed.nodeRef), grabbed.nodeBounds);
}
void BoundsTreePrototype::remove(const void* objectToRemove, const BoundsTemplate<float>& bounds) {
	int resultingBaseSize = removeRecursive(allocator, baseTrunk, baseTrunkSize, objectToRemove, bounds);
	if(resultingBaseSize != -1) {
		this->baseTrunkSize = resultingBaseSize;
	} else {
		throw "Object not found!";
	}
}
bool BoundsTreePrototype::contains(const void* object, const BoundsTemplate<float>& bounds) const {
	return containsObjectRecursive(baseTrunk, baseTrunkSize, object, bounds);
}

bool BoundsTreePrototype::groupContains(const void* object, const BoundsTemplate<float>& bounds, const void* groupRep, const BoundsTemplate<float>& groupRepBounds) const {
	const TreeNodeRef* groupFound = getGroupRecursive(this->baseTrunk, this->baseTrunkSize, groupRep, groupRepBounds);
	if(!groupFound) {
		throw "Group not found!";
	}
	if(groupFound->isTrunkNode()) {
		return containsObjectRecursive(groupFound->asTrunk(), groupFound->getTrunkSize(), object, bounds);
	} else {
		return groupFound->asObject() == object;
	}
}

static bool updateObjectBoundsRecurive(TreeTrunk& curTrunk, int curTrunkSize, const void* object, const BoundsTemplate<float>& originalBounds, const BoundsTemplate<float>& newBounds) {
	assert(curTrunkSize >= 0 && curTrunkSize <= BRANCH_FACTOR);
	std::array<bool, BRANCH_FACTOR> couldContain = TrunkSIMDHelperFallback::getAllContainsBounds(curTrunk, originalBounds);
	for(int i = 0; i < curTrunkSize; i++) {
		if(!couldContain[i]) continue;

		TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			TreeTrunk& subTrunk = subNode.asTrunk();
			int subTrunkSize = subNode.getTrunkSize();
			if(updateObjectBoundsRecurive(subTrunk, subTrunkSize, object, originalBounds, newBounds)) {
				curTrunk.setBoundsOfSubNode(i, TrunkSIMDHelperFallback::getTotalBounds(subTrunk, subTrunkSize));
				return true;
			}
		} else {
			if(subNode.asObject() == object) {
				curTrunk.setBoundsOfSubNode(i, newBounds);
				return true;
			}
		}
	}
	return false;
}

void BoundsTreePrototype::updateObjectBounds(const void* object, const BoundsTemplate<float>& originalBounds, const BoundsTemplate<float>& newBounds) {
	bool wasFound = updateObjectBoundsRecurive(this->baseTrunk, this->baseTrunkSize, object, originalBounds, newBounds);
	if(!wasFound) throw "Object was not found!";
}

static bool findAndReplaceObjectRecursive(TreeTrunk& curTrunk, int curTrunkSize, const void* oldObject, void* newObject, const BoundsTemplate<float>& bounds) {
	assert(curTrunkSize >= 0 && curTrunkSize <= BRANCH_FACTOR);
	std::array<bool, BRANCH_FACTOR> couldContain = TrunkSIMDHelperFallback::getAllContainsBounds(curTrunk, bounds);
	for(int i = 0; i < curTrunkSize; i++) {
		if(!couldContain[i]) continue;

		TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			TreeTrunk& subTrunk = subNode.asTrunk();
			int subTrunkSize = subNode.getTrunkSize();
			if(findAndReplaceObjectRecursive(subTrunk, subTrunkSize, oldObject, newObject, bounds)) {
				return true;
			}
		} else {
			if(subNode.asObject() == oldObject) {
				subNode.setObject(newObject);
				return true;
			}
		}
	}
	return false;
}
void BoundsTreePrototype::findAndReplaceObject(const void* oldObject, void* newObject, const BoundsTemplate<float>& bounds) {
	bool found = findAndReplaceObjectRecursive(this->baseTrunk, this->baseTrunkSize, oldObject, newObject, bounds);
	if(!found) throw "Object to rename not found!";
}

static bool disbandGroupRecursive(TreeTrunk& curTrunk, int curTrunkSize, const void* groupRep, const BoundsTemplate<float>& groupRepBounds) {
	assert(curTrunkSize >= 0 && curTrunkSize <= BRANCH_FACTOR);
	std::array<bool, BRANCH_FACTOR> couldContain = TrunkSIMDHelperFallback::getAllContainsBounds(curTrunk, groupRepBounds);
	for(int i = 0; i < curTrunkSize; i++) {
		if(!couldContain[i]) continue;

		TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			TreeTrunk& subTrunk = subNode.asTrunk();
			int subTrunkSize = subNode.getTrunkSize();
			if(subNode.isGroupHead()) {
				if(containsObjectRecursive(subTrunk, subTrunkSize, groupRep, groupRepBounds)) {
					subNode.removeGroupHead();
					return true;
				}
			} else {
				if(disbandGroupRecursive(subTrunk, subTrunkSize, groupRep, groupRepBounds)) {
					return true;
				}
			}
		} else {
			if(subNode.asObject() == groupRep) {
				return true;
			}
		}
	}
	return false;
}

void BoundsTreePrototype::disbandGroup(const void* groupRep, const BoundsTemplate<float>& groupRepBounds) {
	disbandGroupRecursive(this->baseTrunk, this->baseTrunkSize, groupRep, groupRepBounds);
}

static size_t getSizeRecursive(const TreeTrunk& curTrunk, int curTrunkSize) {
	size_t total = 0;
	for(int i = 0; i < curTrunkSize; i++) {
		const TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			total += getSizeRecursive(subNode.asTrunk(), subNode.getTrunkSize());
		} else {
			total++;
		}
	}
	return total;
}

size_t BoundsTreePrototype::size() const {
	return getSizeRecursive(baseTrunk, baseTrunkSize);
}
size_t BoundsTreePrototype::groupSize(const void* groupRep, const BoundsTemplate<float>& groupRepBounds) const {
	const TreeNodeRef* groupFound = getGroupRecursive(this->baseTrunk, this->baseTrunkSize, groupRep, groupRepBounds);
	if(!groupFound) {
		throw "Group not found!";
	}
	if(groupFound->isTrunkNode()) {
		return getSizeRecursive(groupFound->asTrunk(), groupFound->getTrunkSize());
	} else {
		return 1;
	}
}

void BoundsTreePrototype::clear() {
	this->allocator.freeAllTrunks(this->baseTrunk, this->baseTrunkSize);
	this->baseTrunkSize = 0;
}

};
