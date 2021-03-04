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
BoundsTemplate<float> TreeTrunk::getTotalBounds(int upTo) const {
	assert(upTo > 1 && upTo <= BRANCH_FACTOR);
	BoundsTemplate<float> totalBounds = getBoundsOfSubNode(0);
	for(int i = 1; i < upTo; i++) {
		totalBounds = unionOfBounds(totalBounds, getBoundsOfSubNode(i));
	}
	return totalBounds;
}

std::array<bool, BRANCH_FACTOR> TreeTrunk::getAllContainsBounds(const BoundsTemplate<float>& boundsToContain) const {
	std::array<bool, BRANCH_FACTOR> contained;
	for(int i = 0; i < BRANCH_FACTOR; i++) {
		BoundsTemplate<float> subNodeBounds = this->getBoundsOfSubNode(i);
		contained[i] = subNodeBounds.contains(boundsToContain);
	}
	return contained;
}

std::array<float, BRANCH_FACTOR> TreeTrunk::computeAllCombinationCosts(const BoundsTemplate<float>& boundsExtention) const {
	std::array<float, BRANCH_FACTOR> costs;
	for(int i = 0; i < BRANCH_FACTOR; i++) {
		BoundsTemplate<float> subNodeBounds = this->getBoundsOfSubNode(i);
		costs[i] = unionOfBounds(boundsExtention, subNodeBounds).getVolume() - subNodeBounds.getVolume();
	}
	return costs;
}

int TreeTrunk::getLowestCombinationCost(const BoundsTemplate<float>& boundsExtention, int nodeSize) const {
	std::array<float, BRANCH_FACTOR> costs = this->computeAllCombinationCosts(boundsExtention);
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

void TreeTrunk::moveSubNode(int from, int to) {
	this->setSubNode(to, std::move(this->subNodes[from]), this->getBoundsOfSubNode(from));
}

TreeNodeRef::TreeNodeRef(TreeTrunk* trunk, int trunkSize, bool isGroupHead) noexcept : ptr(reinterpret_cast<std::uintptr_t>(trunk) | (static_cast<std::uintptr_t>(trunkSize) - 1) | (isGroupHead ? BRANCH_FACTOR : 0)) {
	assert(trunkSize >= 2 && trunkSize <= BRANCH_FACTOR); // trunkSize must be between 2-BRANCH_FACTOR
	assert((reinterpret_cast<std::uintptr_t>(trunk) & (BRANCH_FACTOR * sizeof(int64_t) - 1)) == 0); // check trunk is aligned correctly
}
BoundsTemplate<float> TreeNodeRef::getTrunkBounds() const {
	assert(this->ptr != INVALID_REF);
	assert(isTrunkNode());
	return this->asTrunk().getTotalBounds(this->getTrunkSize());
}
const TreeTrunk& TreeNodeRef::asTrunk() const {
	assert(this->ptr != INVALID_REF);
	assert(isTrunkNode());
	return *reinterpret_cast<TreeTrunk*>(ptr & PTR_MASK);
}
TreeTrunk& TreeNodeRef::asTrunk() {
	assert(this->ptr != INVALID_REF);
	assert(isTrunkNode());
	return *reinterpret_cast<TreeTrunk*>(ptr & PTR_MASK);
}
void* TreeNodeRef::asObject() const {
	assert(this->ptr != INVALID_REF);
	assert(isLeafNode());
	return reinterpret_cast<TreeTrunk*>(ptr & ~SIZE_DATA_MASK);
}

// returns the new size of this node, to be applied to the caller
int addRecursive(TrunkAllocator& allocator, TreeTrunk& curTrunk, int curTrunkSize, TreeNodeRef&& newNode, const BoundsTemplate<float>& bounds) {
	assert(curTrunkSize >= 0 && curTrunkSize <= BRANCH_FACTOR);
	if(curTrunkSize == BRANCH_FACTOR) {
		int chosenNode = curTrunk.getLowestCombinationCost(bounds, curTrunkSize);

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
	std::array<bool, BRANCH_FACTOR> couldContain = trunk.getAllContainsBounds(bounds);
	for(int i = 0; i < trunkSize; i++) {
		if(couldContain[i]) {
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
	}
	return false;
}

// returns new trunkSize if removed, -1 if not removed
int removeRecursive(TrunkAllocator& allocator, TreeTrunk& curTrunk, int curTrunkSize, const void* objectToRemove, const BoundsTemplate<float>& bounds) {
	assert(curTrunkSize >= 0 && curTrunkSize <= BRANCH_FACTOR);
	std::array<bool, BRANCH_FACTOR> couldContain = curTrunk.getAllContainsBounds(bounds);
	for(int i = 0; i < curTrunkSize; i++) {
		if(couldContain[i]) {
			TreeNodeRef& subNode = curTrunk.subNodes[i];
			if(subNode.isTrunkNode()) {
				TreeTrunk& subNodeTrunk = subNode.asTrunk();
				assert(subNode.getTrunkSize() >= 2);
				int newSubNodeTrunkSize = removeRecursive(allocator, subNodeTrunk, subNode.getTrunkSize(), objectToRemove, bounds);
				assert(newSubNodeTrunkSize >= 1 || newSubNodeTrunkSize == -1);
				if(newSubNodeTrunkSize != -1) {
					if(newSubNodeTrunkSize == 1) {
						// remove reduntant trunk
						curTrunk.setSubNode(i, std::move(subNodeTrunk.subNodes[0]), subNodeTrunk.getBoundsOfSubNode(0));
						allocator.freeTrunk(&subNodeTrunk);
					} else {
						curTrunk.setBoundsOfSubNode(i, subNodeTrunk.getTotalBounds(newSubNodeTrunkSize));
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
	std::array<bool, BRANCH_FACTOR> couldContain = curTrunk.getAllContainsBounds(representativeBounds);
	for(int i = 0; i < curTrunkSize; i++) {
		if(couldContain[i]) {
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
								curTrunk.setBoundsOfSubNode(i, subNodeTrunk.getTotalBounds(newSubNodeTrunkSize));
								subNode.setTrunkSize(newSubNodeTrunkSize);
							}

							recursiveResult.resultingGroupSize = curTrunkSize;
							return recursiveResult;
						}
					}
				}

			} else {
				if(subNode.asObject() == groupRepresentative) {
					TreeNodeRef subNodeCopy = std::move(subNode);
					curTrunk.moveSubNode(curTrunkSize - 1, i);
					return TreeGrab(curTrunkSize - 1, std::move(subNodeCopy), curTrunk.getBoundsOfSubNode(i));
				}
			}
		}
	}
	return TreeGrab();
}

const TreeNodeRef* getGroupRecursive(const TreeTrunk& curTrunk, int curTrunkSize, const void* groupRepresentative, const BoundsTemplate<float>& representativeBounds) {
	assert(curTrunkSize >= 0 && curTrunkSize <= BRANCH_FACTOR);
	std::array<bool, BRANCH_FACTOR> couldContain = curTrunk.getAllContainsBounds(representativeBounds);
	for(int i = 0; i < curTrunkSize; i++) {
		if(couldContain[i]) {
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
	}
	return nullptr;
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
			forEachTrunkRecurse(groupToDestroy.asTrunk(), groupToDestroy.getTrunkSize(), [&sourceAlloc](TreeTrunk& t) {
				sourceAlloc.freeTrunk(&t);
			});
		}

		return trunk->getTotalBounds(trunkSize);
	});
	return groupWasFound;
}



TreeTrunk* TrunkAllocator::allocTrunk() const {
	return static_cast<TreeTrunk*>(aligned_malloc(sizeof(TreeTrunk), alignof(TreeTrunk)));
}
void TrunkAllocator::freeTrunk(TreeTrunk* trunk) const {
	aligned_free(trunk);
}

BoundsTree::BoundsTree() : baseTrunk(), baseTrunkSize(0) {}
BoundsTree::~BoundsTree() {
	this->clear();
}
void BoundsTree::add(void* newObject, const BoundsTemplate<float>& bounds) {
	this->baseTrunkSize = P3D::NewBoundsTree::addRecursive(allocator, baseTrunk, baseTrunkSize, TreeNodeRef(newObject), bounds);
}
void BoundsTree::addToGroup(void* newObject, const BoundsTemplate<float>& newObjectBounds, void* groupRepresentative, const BoundsTemplate<float>& groupRepBounds) {
	bool foundGroup = modifyGroupRecursive(allocator, baseTrunk, baseTrunkSize, groupRepresentative, groupRepBounds, [this, newObject, &newObjectBounds](TreeNodeRef& groupNode, const BoundsTemplate<float>& groupNodeBounds) {
		assert(groupNode.isGroupHeadOrLeaf());
		if(groupNode.isTrunkNode()) {
			TreeTrunk& groupTrunk = groupNode.asTrunk();
			int resultingSize = addRecursive(allocator, groupTrunk, groupNode.getTrunkSize(), TreeNodeRef(newObject), newObjectBounds);
			groupNode.setTrunkSize(resultingSize);
			return groupTrunk.getTotalBounds(resultingSize);
		} else {
			TreeTrunk* newTrunkNode = allocator.allocTrunk();
			newTrunkNode->setSubNode(0, std::move(groupNode), groupNodeBounds);
			newTrunkNode->setSubNode(1, TreeNodeRef(newObject), newObjectBounds);
			groupNode = TreeNodeRef(newTrunkNode, 2, true);
			return newTrunkNode->getTotalBounds(2);
		}
	});
	if(!foundGroup) {
		throw "Group not found!";
	}
}
void BoundsTree::mergeGroups(const void* groupRepA, const BoundsTemplate<float>& repABounds, const void* groupRepB, const BoundsTemplate<float>& repBBounds) {
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

void BoundsTree::transferGroupTo(const void* groupRep, const BoundsTemplate<float>& groupRepBounds, BoundsTree& destinationTree) {
	TreeGrab grabbed = grabGroupRecursive(this->allocator, this->baseTrunk, this->baseTrunkSize, groupRep, groupRepBounds);
	if(grabbed.resultingGroupSize == -1) {
		throw "groupRep not found!";
	}
	this->baseTrunkSize = grabbed.resultingGroupSize;

	bool groupBWasFound = addRecursive(destinationTree.allocator, destinationTree.baseTrunk, destinationTree.baseTrunkSize, std::move(grabbed.nodeRef), grabbed.nodeBounds);
}
void BoundsTree::remove(const void* objectToRemove, const BoundsTemplate<float>& bounds) {
	int resultingBaseSize = removeRecursive(allocator, baseTrunk, baseTrunkSize, objectToRemove, bounds);
	if(resultingBaseSize != -1) {
		this->baseTrunkSize = resultingBaseSize;
	} else {
		throw "Object not found!";
	}
}
bool BoundsTree::contains(const void* object, const BoundsTemplate<float>& bounds) const {
	return containsObjectRecursive(baseTrunk, baseTrunkSize, object, bounds);
}

bool BoundsTree::groupContains(const void* object, const BoundsTemplate<float>& bounds, const void* groupRep, const BoundsTemplate<float>& groupRepBounds) const {
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

size_t BoundsTree::size() const {
	size_t total = 0;
	this->forEach([&total](const void* item) {
		total++;
	});
	return total;
}
size_t BoundsTree::groupSize(const void* groupRep, const BoundsTemplate<float>& groupRepBounds) const {
	size_t total = 0;
	this->forEachInGroup(groupRep, groupRepBounds, [&total](const void* item) {
		total++;
	});
	return total;
}

void BoundsTree::clear() {
	forEachTrunkRecurse(this->baseTrunk, this->baseTrunkSize, [this](TreeTrunk& trunk) {
		allocator.freeTrunk(&trunk);
	});
}

};
