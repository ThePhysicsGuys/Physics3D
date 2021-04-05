#include "boundsTree2.h"


#include "aligned_alloc.h"

namespace P3D::NewBoundsTree {

// naive implementation, to be optimized
BoundsTemplate<float> TrunkSIMDHelperFallback::getTotalBounds(const TreeTrunk& trunk, int upTo) {
	assert(upTo >= 1 && upTo <= BRANCH_FACTOR);
	BoundsTemplate<float> totalBounds = trunk.getBoundsOfSubNode(0);
	for(int i = 1; i < upTo; i++) {
		totalBounds = unionOfBounds(totalBounds, trunk.getBoundsOfSubNode(i));
	}
	return totalBounds;
}
// naive implementation, to be optimized
BoundsTemplate<float> TrunkSIMDHelperFallback::getTotalBoundsWithout(const TreeTrunk& trunk, int upTo, int without) {
	assert(upTo >= 2 && upTo <= BRANCH_FACTOR); // size must be at least 2, can't compute otherwise
	assert(without >= 0 && without < BRANCH_FACTOR);
	if(without == 0) {
		BoundsTemplate<float> totalBounds = trunk.getBoundsOfSubNode(1);
		for(int i = 2; i < upTo; i++) {
			totalBounds = unionOfBounds(totalBounds, trunk.getBoundsOfSubNode(i));
		}
		return totalBounds;
	} else {
		BoundsTemplate<float> totalBounds = trunk.getBoundsOfSubNode(0);
		for(int i = 1; i < upTo; i++) {
			if(i == without) continue;
			totalBounds = unionOfBounds(totalBounds, trunk.getBoundsOfSubNode(i));
		}
		return totalBounds;
	}
}

BoundsArray<BRANCH_FACTOR> TrunkSIMDHelperFallback::getAllTotalBoundsWithout(const TreeTrunk& trunk, int upTo) {
	assert(upTo >= 2 && upTo <= BRANCH_FACTOR); // size must be at least 2, can't compute otherwise
	BoundsArray<BRANCH_FACTOR> result;
	{
		BoundsTemplate<float> totalBounds0 = trunk.getBoundsOfSubNode(1);
		for(int i = 2; i < upTo; i++) {
			totalBounds0 = unionOfBounds(totalBounds0, trunk.getBoundsOfSubNode(i));
		}
		result.setBounds(0, totalBounds0);
	}
	for(int without = 1; without < upTo; without++) {
		BoundsTemplate<float> totalBounds = trunk.getBoundsOfSubNode(0);
		for(int i = 2; i < upTo; i++) {
			if(i == without) continue;
			totalBounds = unionOfBounds(totalBounds, trunk.getBoundsOfSubNode(i));
		}
		result.setBounds(without, totalBounds);
	}

	return result;
}

std::array<bool, BRANCH_FACTOR> TrunkSIMDHelperFallback::getAllContainsBounds(const TreeTrunk& trunk, const BoundsTemplate<float>& boundsToContain) {
	std::array<bool, BRANCH_FACTOR> contained;
	for(int i = 0; i < BRANCH_FACTOR; i++) {
		BoundsTemplate<float> subNodeBounds = trunk.getBoundsOfSubNode(i);
		contained[i] = subNodeBounds.contains(boundsToContain);
	}
	return contained;
}

std::array<float, BRANCH_FACTOR> TrunkSIMDHelperFallback::computeAllCosts(const TreeTrunk& trunk) {
	std::array<float, BRANCH_FACTOR> costs;
	for(int i = 0; i < BRANCH_FACTOR; i++) {
		BoundsTemplate<float> subNodeBounds = trunk.getBoundsOfSubNode(i);
		costs[i] = computeCost(subNodeBounds);
	}
	return costs;
}

std::array<float, BRANCH_FACTOR> TrunkSIMDHelperFallback::computeAllCombinationCosts(const BoundsArray<BRANCH_FACTOR>& boundsArr, const BoundsTemplate<float>& boundsExtention) {
	std::array<float, BRANCH_FACTOR> costs;
	for(int i = 0; i < BRANCH_FACTOR; i++) {
		BoundsTemplate<float> subNodeBounds = boundsArr.getBounds(i);
		costs[i] = computeCost(unionOfBounds(boundsExtention, subNodeBounds));
	}
	return costs;
}

std::pair<int, int> TrunkSIMDHelperFallback::computeFurthestObjects(const BoundsArray<BRANCH_FACTOR * 2>& boundsArray, int size) {
	std::pair<int, int> furthestObjects{0, 1};
	float biggestCost = -std::numeric_limits<float>::infinity();
	for(int i = 0; i < size - 1; i++) {
		BoundsTemplate<float> iBounds = boundsArray.getBounds(i);
		for(int j = i + 1; j < size; j++) {
			BoundsTemplate<float> jBounds = boundsArray.getBounds(j);

			float cost = computeCost(unionOfBounds(iBounds, jBounds));
			if(cost > biggestCost) {
				biggestCost = cost;
				furthestObjects.first = i;
				furthestObjects.second = j;
			}
		}
	}

	return furthestObjects;
}

int TrunkSIMDHelperFallback::getLowestCombinationCost(const TreeTrunk& trunk, const BoundsTemplate<float>& boundsExtention, int nodeSize) {
	std::array<float, BRANCH_FACTOR> costs = TrunkSIMDHelperFallback::computeAllCombinationCosts(trunk.subNodeBounds, boundsExtention);
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

OverlapMatrix TrunkSIMDHelperFallback::computeBoundsOverlapMatrix(const TreeTrunk& trunkA, int trunkASize, const TreeTrunk& trunkB, int trunkBSize) {
	OverlapMatrix result;
	for(int a = 0; a < trunkASize; a++) {
		BoundsTemplate<float> aBounds = trunkA.getBoundsOfSubNode(a);
		for(int b = 0; b < trunkBSize; b++) {
			BoundsTemplate<float> bBounds = trunkB.getBoundsOfSubNode(b);
			result[a][b] = intersects(aBounds, bBounds);
		}
	}
	return result;
}

OverlapMatrix TrunkSIMDHelperFallback::computeInternalBoundsOverlapMatrix(const TreeTrunk& trunk, int trunkSize) {
	OverlapMatrix result;
	for(int a = 0; a < trunkSize; a++) {
		BoundsTemplate<float> aBounds = trunk.getBoundsOfSubNode(a);
		for(int b = a+1; b < trunkSize; b++) {
			BoundsTemplate<float> bBounds = trunk.getBoundsOfSubNode(b);
			result[a][b] = intersects(aBounds, bBounds);
		}
	}
	return result;
}

std::array<float, BRANCH_FACTOR> TrunkSIMDHelperFallback::computeAllExtentionCosts(const TreeTrunk& trunk, int trunkSize, const BoundsTemplate<float>& extraBounds) {
	std::array<float, BRANCH_FACTOR> resultingCosts;
	for(int i = 0; i < trunkSize; i++) {
		resultingCosts[i] = computeAdditionCost(trunk.getBoundsOfSubNode(i), extraBounds);
	}
	return resultingCosts;
}

int TrunkSIMDHelperFallback::transferNodes(TreeTrunk& srcTrunk, int srcTrunkStart, int srcTrunkEnd, TreeTrunk& destTrunk, int destTrunkSize) {
	for(int i = srcTrunkStart; i < srcTrunkEnd; i++) {
		destTrunk.setSubNode(destTrunkSize, std::move(srcTrunk.subNodes[i]), srcTrunk.getBoundsOfSubNode(i));
		destTrunkSize++;
	}
	return destTrunkSize;
}

BoundsArray<BRANCH_FACTOR * 2> TrunkSIMDHelperFallback::combineBoundsArrays(const TreeTrunk& trunkA, int trunkASize, const TreeTrunk& trunkB, int trunkBSize) {
	BoundsArray<BRANCH_FACTOR * 2> result;
	for(int i = 0; i < trunkASize; i++) {
		result.setBounds(i, trunkA.getBoundsOfSubNode(i));
	}
	for(int i = 0; i < trunkBSize; i++) {
		result.setBounds(trunkASize + i, trunkB.getBoundsOfSubNode(i));
	}
	return result;
}

// returns true if modified
bool TrunkSIMDHelperFallback::exchangeNodesBetween(TreeTrunk& trunkA, int& trunkASize, TreeTrunk& trunkB, int& trunkBSize) {
	int totalSize = trunkASize + trunkBSize;
	// if this is not the case, we could've just merged the nodes, this is handled in another function
	// if totalSize was BRANCH_FACTOR + 1 we could've moved all but one node from one trunk to the other, also handled elsewhere
	assert(totalSize >= BRANCH_FACTOR + 2); 
	BoundsArray<BRANCH_FACTOR * 2> allBounds = TrunkSIMDHelperFallback::combineBoundsArrays(trunkA, trunkASize, trunkB, trunkBSize);
	std::pair<int, int> furthestObjects = TrunkSIMDHelperFallback::computeFurthestObjects(allBounds, totalSize);

	BoundsTemplate<float> aBounds = allBounds.getBounds(furthestObjects.first);
	BoundsTemplate<float> bBounds = allBounds.getBounds(furthestObjects.second);

	int aResultSize = 0; 
	int bResultSize = 0; 
	int aResult[BRANCH_FACTOR * 2];
	int bResult[BRANCH_FACTOR * 2];

	// positive if costA < costB
	float allDeltaCosts[BRANCH_FACTOR * 2];
	for(int i = 0; i < totalSize; i++) {
		BoundsTemplate<float> bounds = allBounds.getBounds(i);
		float costA = computeCost(unionOfBounds(aBounds, bounds));
		float costB = computeCost(unionOfBounds(bBounds, bounds));

		allDeltaCosts[i] = costB - costA;
	}

	for(int i = 0; i < totalSize; i++) {
		if(allDeltaCosts[i] >= 0.0) {
			aResult[aResultSize++] = i;
		} else {
			bResult[bResultSize++] = i;
		}
	}

	if(aResultSize > BRANCH_FACTOR) {
		// move least costly elements from a to b (least costly cost is closest to 0)
		do {
			int worstIndex = 0; 
			float worstCost = allDeltaCosts[aResult[0]];
			for(int i = 1; i < aResultSize; i++) {
				float cost = allDeltaCosts[aResult[i]];
				if(cost < worstCost) {
					worstIndex = i;
					worstCost = cost;
				}
			}
			bResult[bResultSize++] = aResult[worstIndex];
			aResult[worstIndex] = aResult[--aResultSize];
		} while(aResultSize > BRANCH_FACTOR);
	} else if(bResultSize > BRANCH_FACTOR) {
		// move least costly elements from b to a (least costly cost is closest to 0)
		do {
			int worstIndex = 0;
			float worstCost = allDeltaCosts[bResult[0]];
			for(int i = 1; i < bResultSize; i++) {
				float cost = allDeltaCosts[bResult[i]];
				if(cost > worstCost) { // these costs are all negative
					worstIndex = i;
					worstCost = cost;
				}
			}
			aResult[aResultSize++] = bResult[worstIndex];
			bResult[worstIndex] = bResult[--bResultSize];
		} while(bResultSize > BRANCH_FACTOR);
	}

	assert(aResultSize + bResultSize == totalSize);
	assert(aResultSize <= BRANCH_FACTOR && bResultSize <= BRANCH_FACTOR);
	assert(aResultSize >= 2 && bResultSize >= 2); // Guaranteed by totalSize >= BRANCH_FACTOR + 2 && aResultSize <= BRANCH_FACTOR && bResultSize <= BRANCH_FACTOR

	// checks to see if a change needs to be made
	bool a0ComesFromA = aResult[0] < trunkASize; 
	bool b0ComesFromA = bResult[0] < trunkASize;
	if(a0ComesFromA != b0ComesFromA) {// they differ in origin
		for(int i = 1; i < aResultSize; i++) {
			bool comesFromA = aResult[i] < trunkASize;
			if(comesFromA != a0ComesFromA) {
				goto wasChanged;
			}
		}
		for(int i = 1; i < bResultSize; i++) {
			bool comesFromA = bResult[i] < trunkASize;
			if(comesFromA != b0ComesFromA) {
				goto wasChanged;
			}
		}
		return false;
	}
	wasChanged:;

	TreeNodeRef allSubNodes[BRANCH_FACTOR * 2];
	for(int i = 0; i < trunkASize; i++) {
		allSubNodes[i] = std::move(trunkA.subNodes[i]);
	}
	for(int i = 0; i < trunkBSize; i++) {
		allSubNodes[i+trunkASize] = std::move(trunkB.subNodes[i]);
	}

	for(int i = 0; i < aResultSize; i++) {
		int from = aResult[i];
		trunkA.setSubNode(i, std::move(allSubNodes[from]), allBounds.getBounds(from));
	}
	for(int i = 0; i < bResultSize; i++) {
		int from = bResult[i];
		trunkB.setSubNode(i, std::move(allSubNodes[from]), allBounds.getBounds(from));
	}

	trunkASize = aResultSize;
	trunkBSize = bResultSize;
	return true;
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

static void improveTrunkHorizontalOld(TreeTrunk& trunk, int trunkSize) {
	TreeNodeRef allSubSubNodes[BRANCH_FACTOR * BRANCH_FACTOR];
	BoundsArray<BRANCH_FACTOR* BRANCH_FACTOR> allSubNodeBounds;
	int subSubNodeCount = 0;

	for(int i = 0; i < trunkSize; i++) {
		TreeNodeRef& subNode = trunk.subNodes[i];

		if(subNode.isTrunkNode() && !subNode.isGroupHead()) { // no redistributing groups
			TreeTrunk& subTrunk = subNode.asTrunk();
			int subNodeSize = subNode.getTrunkSize();

			for(int subI = 0; subI < subNodeSize; subI++) {
				allSubSubNodes[subSubNodeCount] = std::move(subTrunk.subNodes[subI]);
				allSubNodeBounds.setBounds(subSubNodeCount, subTrunk.getBoundsOfSubNode(i));
				subSubNodeCount++;
			}
		} else {
			allSubSubNodes[subSubNodeCount] = std::move(subNode);
			allSubNodeBounds.setBounds(subSubNodeCount, trunk.getBoundsOfSubNode(i));
			subSubNodeCount++;
		}
	}

	// 0 - 1  - 2  - 3  - 4  - 5  - 6  - 7  - 8  Num TrunkNodes
	// 0 - 9  - 16 - 23 - 30 - 37 - 44 - 51 - 58 Min to trunk
	// 8 - 15 - 22 - 29 - 36 - 43 - 50 - 57 - 64 Max to trunk

	// 0 - 1 - 2  - 3  - 4
	// 0 - 5 - 8  - 11 - 14
	// 4 - 7 - 10 - 13 - 16
	int numberOfResultingTrunks = (subSubNodeCount - 2) / (BRANCH_FACTOR - 1);
	int numberOfFreeNodes = subSubNodeCount - numberOfResultingTrunks * BRANCH_FACTOR;


}

// means that this trunk and it's subtrunks cannot be improved
static bool isLeafTrunk(TreeTrunk& trunk, int trunkSize) {
	for(int subNodeI = 0; subNodeI < trunkSize; subNodeI++) {
		TreeNodeRef& subNode = trunk.subNodes[subNodeI];
		if(!subNode.isGroupHeadOrLeaf()) return false;
	}
	return true;
}

static std::pair<int, float> findBestMovingCost(TreeTrunk& trunk, int trunkSize, TreeTrunk& subTrunk, int subTrunkSize, int subNodeI, int subSubNodeI) {
	BoundsTemplate<float> extraBounds = subTrunk.getBoundsOfSubNode(subSubNodeI);

	int wantsToMoveTo = -1;
	float bestCost = computeAdditionCost(TrunkSIMDHelperFallback::getTotalBoundsWithout(subTrunk, subTrunkSize, subSubNodeI), extraBounds);

	std::array<float, BRANCH_FACTOR> allCosts = TrunkSIMDHelperFallback::computeAllExtentionCosts(trunk, trunkSize, extraBounds);

	for(int i = 0; i < trunkSize; i++) {
		if(i == subNodeI) continue;
		if(allCosts[i] < bestCost) {
			bestCost = allCosts[i];
			wantsToMoveTo = i;
		}
	}

	return std::pair<int, float>(wantsToMoveTo, bestCost);
}

static void improveTrunkHorizontal(TrunkAllocator& alloc, TreeTrunk& trunk) {
	constexpr int trunkSize = BRANCH_FACTOR; // required by the interface, allows for small SIMD optimizations

	assert(!isLeafTrunk(trunk, trunkSize));

	// indexed overlaps[i][j] with j >= i+1
	OverlapMatrix overlaps = TrunkSIMDHelperFallback::computeInternalBoundsOverlapMatrix(trunk, trunkSize);

	for(int subNodeAI = 0; subNodeAI < trunkSize-1; subNodeAI++) {
		TreeNodeRef& subNodeA = trunk.subNodes[subNodeAI];

		if(subNodeA.isGroupHeadOrLeaf()) continue; // no breaking up groups

		TreeTrunk& subTrunkA = subNodeA.asTrunk();
		int subTrunkSizeA = subNodeA.getTrunkSize();

		for(int subNodeBI = subNodeAI+1; subNodeBI < trunkSize - 1; subNodeBI++) {
			TreeNodeRef& subNodeB = trunk.subNodes[subNodeBI];

			if(subNodeB.isGroupHeadOrLeaf()) continue; // no breaking up groups

			TreeTrunk& subTrunkB = subNodeB.asTrunk();
			int subTrunkSizeB = subNodeB.getTrunkSize();

			if(!overlaps[subNodeAI][subNodeBI]) continue;

			if(subTrunkSizeA + subTrunkSizeB >= BRANCH_FACTOR + 2) {
				// if true, this updates subTrunkSizeA and subTrunkSizeB
				if(TrunkSIMDHelperFallback::exchangeNodesBetween(subTrunkA, subTrunkSizeA, subTrunkB, subTrunkSizeB)) {
					// update treenoderefs
					subNodeA.setTrunkSize(subTrunkSizeA);
					subNodeB.setTrunkSize(subTrunkSizeB);

					trunk.setBoundsOfSubNode(subNodeAI, TrunkSIMDHelperFallback::getTotalBounds(subTrunkA, subTrunkSizeA));
					trunk.setBoundsOfSubNode(subNodeBI, TrunkSIMDHelperFallback::getTotalBounds(subTrunkB, subTrunkSizeB));
					// just return, changing trunkNodes invalidated overlaps matrix, more improvements can be done in future calls
					return;
				}
			} else {
				// can just merge out a Trunk
				// move all but first element from B to A
				subTrunkSizeA = TrunkSIMDHelperFallback::transferNodes(subTrunkB, 1, subTrunkSizeB, subTrunkA, subTrunkSizeA);
				subNodeA.setTrunkSize(subTrunkSizeA);
				trunk.setBoundsOfSubNode(subNodeAI, TrunkSIMDHelperFallback::getTotalBounds(subTrunkA, subTrunkSizeA));
				trunk.setSubNode(subNodeBI, std::move(subTrunkB.subNodes[0]), subTrunkB.getBoundsOfSubNode(0));
				alloc.freeTrunk(&subTrunkB);

				// just return, changing trunkNodes invalidated overlaps matrix, more improvements can be done in future calls
				return;
			}
		}
	}
}

inline static void swapNodesBetweenTrunks(TreeTrunk& trunkA, int indexInTrunkA, TreeTrunk& trunkB, int indexInTrunkB) {
	BoundsTemplate<float> boundsA = trunkA.getBoundsOfSubNode(indexInTrunkA);
	BoundsTemplate<float> boundsB = trunkB.getBoundsOfSubNode(indexInTrunkB);

	trunkA.setBoundsOfSubNode(indexInTrunkA, boundsB);
	trunkB.setBoundsOfSubNode(indexInTrunkB, boundsA);

	std::swap(trunkA.subNodes[indexInTrunkA], trunkB.subNodes[indexInTrunkB]);
}

static void improveTrunkVertical(TreeTrunk& trunk) {
	constexpr int trunkSize = BRANCH_FACTOR; // required by the interface, allows for small SIMD optimizations

	if(isLeafTrunk(trunk, trunkSize)) return; // no improvement possible

	std::array<float, BRANCH_FACTOR> allExistingSizes = TrunkSIMDHelperFallback::computeAllCosts(trunk);

	int smallestSubNodeI = 0;
	float smallestSubNodeSize = allExistingSizes[0];

	for(int i = 1; i < trunkSize; i++) {
		if(allExistingSizes[i] < smallestSubNodeSize) {
			smallestSubNodeSize = allExistingSizes[i];
			smallestSubNodeI = i;
		}
	}

	int largestItemSubTrunkI = -1;
	int largestItemI = -1;
	float largestItemInSubTrunkISize = smallestSubNodeSize; // try to beat this, if we can't then there's no swap

	for(int subTrunkI = 0; subTrunkI < trunkSize; subTrunkI++) {
		if(subTrunkI == smallestSubNodeI) continue; // the smallest node cannot contain a node larger than itself, also, trying to swap these would store this node in itself, leading to memory leak and objects disappearing
		TreeNodeRef& subNode = trunk.subNodes[subTrunkI];

		// find a node, and try to swap it with an element from a group
		if(subNode.isGroupHeadOrLeaf()) continue;

		TreeTrunk& subTrunk = subNode.asTrunk();
		int subTrunkSize = subNode.getTrunkSize();

		std::array<float, BRANCH_FACTOR> subTrunkSizes = TrunkSIMDHelperFallback::computeAllCosts(subTrunk);
		
		for(int i = 0; i < subTrunkSize; i++) {
			if(subTrunkSizes[i] > largestItemInSubTrunkISize) {
				largestItemInSubTrunkISize = subTrunkSizes[i];
				largestItemSubTrunkI = subTrunkI;
				largestItemI = i;
			}
		}
	}

	if(largestItemI != -1) {
		// an improvement can be made by swapping
		TreeTrunk& chosenSubTrunk = trunk.subNodes[largestItemSubTrunkI].asTrunk();
		int chosenSubTrunkSize = trunk.subNodes[largestItemSubTrunkI].getTrunkSize();
		swapNodesBetweenTrunks(trunk, smallestSubNodeI, chosenSubTrunk, largestItemI);
		trunk.setBoundsOfSubNode(largestItemSubTrunkI, TrunkSIMDHelperFallback::getTotalBounds(chosenSubTrunk, chosenSubTrunkSize));
	}
}

static int moveElementsOutOfGroup(TrunkAllocator& alloc, TreeTrunk& trunk, int trunkSize) {
	assert(!isLeafTrunk(trunk, trunkSize));
	if(trunkSize >= BRANCH_FACTOR) return trunkSize;
	int freeSlots = BRANCH_FACTOR - trunkSize;

	for(int subNodeI = 0; subNodeI < trunkSize; subNodeI++) {
		TreeNodeRef& subNode = trunk.subNodes[subNodeI];

		// find a node, and try to swap it with an element from a group
		if(!subNode.isGroupHeadOrLeaf()) { // no breaking up groups
			TreeTrunk& subTrunk = subNode.asTrunk();
			int subTrunkSize = subNode.getTrunkSize();

			if(subTrunkSize <= freeSlots + 1) {
				// whole trunk is consumed, can use an extra slot since 

				trunk.setSubNode(subNodeI, std::move(subTrunk.subNodes[0]), subTrunk.getBoundsOfSubNode(0));

				trunkSize = TrunkSIMDHelperFallback::transferNodes(subTrunk, 1, subTrunkSize, trunk, trunkSize);
				alloc.freeTrunk(&subTrunk);

				if(trunkSize >= BRANCH_FACTOR) return trunkSize;
				freeSlots = BRANCH_FACTOR - trunkSize;
			} else {
				// not consuming the whole trunk add what we can
				int resultingSubTrunkSize = subTrunkSize - freeSlots;
				int resultingSize = TrunkSIMDHelperFallback::transferNodes(subTrunk, resultingSubTrunkSize, subTrunkSize, trunk, trunkSize);
				subNode.setTrunkSize(resultingSubTrunkSize);
				trunk.setBoundsOfSubNode(subNodeI, TrunkSIMDHelperFallback::getTotalBounds(subTrunk, resultingSubTrunkSize));
				assert(resultingSize == BRANCH_FACTOR);
				return resultingSize;
			}
		}
	}

	return trunkSize;
}

static int improveStructureRecursive(TrunkAllocator& alloc, TreeTrunk& trunk, int trunkSize) {
	bool givenTrunkIsLeafTrunk = true;
	for(int i = 0; i < trunkSize; i++) {
		TreeNodeRef& subNode = trunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			TreeTrunk& subTrunk = subNode.asTrunk();
			int subTrunkSize = subNode.getTrunkSize();

			subTrunkSize = improveStructureRecursive(alloc, subTrunk, subTrunkSize);

			subNode.setTrunkSize(subTrunkSize);

			if(!subNode.isGroupHead()) givenTrunkIsLeafTrunk = false;
		}
	}
	if(givenTrunkIsLeafTrunk) return trunkSize;

	trunkSize = moveElementsOutOfGroup(alloc, trunk, trunkSize);

	if(trunkSize != BRANCH_FACTOR) return trunkSize;
	if(isLeafTrunk(trunk, trunkSize)) return trunkSize;

	improveTrunkVertical(trunk); // trunkSize == BRANCH_FACTOR
	improveTrunkHorizontal(alloc, trunk); // trunkSize == BRANCH_FACTOR

	return trunkSize;
}
void BoundsTreePrototype::improveStructure() {
	improveStructureRecursive(this->allocator, this->baseTrunk, this->baseTrunkSize);
}
void BoundsTreePrototype::maxImproveStructure() {
	for(int i = 0; i < 5; i++) {
		this->improveStructure();
	}
}

};
