#pragma once

#include "../math/fix.h"
#include "../math/position.h"
#include "../math/bounds.h"
#include "aligned_alloc.h"

#include <cstdint>
#include <utility>
#include <cassert>
#include <limits>

namespace P3D::NewBoundsTree {

constexpr int BRANCH_FACTOR = 8;
static_assert((BRANCH_FACTOR & (BRANCH_FACTOR - 1)) == 0, "Branch factor must be power of 2");

class TreeTrunk;
class BoundsTree;
class TreeNodeRef;

class TreeNodeRef {
	friend class TreeTrunk;
	friend class BoundsTree;

	/* encoding:
		0b...pppppgsss

		(this is if BRANCH_FACTOR == 8)
		Last 3 bits specify type: 
		0b000: leaf node -> ptr points to object
		else : trunk node -> ptr points to TreeTrunk
			g bit specifies 'isGroupHead'
			s bits specify size of this trunk node - 1. 0b111 is a trunknode of size 8
	*/
	std::intptr_t ptr;

	inline int getSizeData() const {
		return static_cast<int>(ptr & (BRANCH_FACTOR - 1));
	}

	inline BoundsTemplate<float> getTrunkBounds() const;

public:
#ifndef NDEBUG
	TreeNodeRef() : ptr(0xADADADADADADADAD) {}
#else
	TreeNodeRef() = default;
#endif
	inline explicit TreeNodeRef(TreeTrunk& trunk, int trunkSize, bool isGroupHead);
	inline explicit TreeNodeRef(void* ptr) : ptr(reinterpret_cast<std::intptr_t>(ptr)) {
		assert(reinterpret_cast<std::intptr_t>(ptr) & (BRANCH_FACTOR - 1) == 0);
	}
	inline bool isLeafNode() const {
		return getSizeData() == 0;
	}
	inline bool isTrunkNode() const {
		return getSizeData() != 0;
	}
	inline int getTrunkSize() const {
		assert(isTrunkNode());
		return getSizeData() + 1;
	}
	inline bool isGroupHead() const {
		assert(isTrunkNode());
		return (ptr & BRANCH_FACTOR) != 0;
	}
	inline TreeTrunk& asTrunk() const;
	inline void* asObject() const;

	// expects a function of the form BoundsTemplate<float>(const CastTo&)
	template<typename CastTo, typename GetObjectBoundsFunc>
	inline BoundsTemplate<float> recalculateBoundsRecursive(const GetObjectBoundsFunc& getObjBounds);
};

class alignas(64) TreeTrunk {
	friend class TreeNodeRef;
	friend class BoundsTree;

	float xMin[BRANCH_FACTOR];
	float yMin[BRANCH_FACTOR];
	float zMin[BRANCH_FACTOR];
	float xMax[BRANCH_FACTOR];
	float yMax[BRANCH_FACTOR];
	float zMax[BRANCH_FACTOR];
	TreeNodeRef subNodes[BRANCH_FACTOR];
public:
	inline BoundsTemplate<float> getBoundsOfSubNode(int subNode) const {
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

	inline void setBoundsOfSubNode(int subNode, BoundsTemplate<float> newBounds) {
		assert(subNode >= 0 && subNode < BRANCH_FACTOR);
		xMin[subNode] = newBounds.min.x;
		yMin[subNode] = newBounds.min.y;
		zMin[subNode] = newBounds.min.z;
		xMax[subNode] = newBounds.max.x;
		yMax[subNode] = newBounds.max.y;
		zMax[subNode] = newBounds.max.z;
	}

	inline void setSubNode(int subNode, BoundsTemplate<float> newBounds, TreeNodeRef&& newNode) {
		assert(subNode >= 0 && subNode < BRANCH_FACTOR);
		subNodes[subNode] = std::move(newNode);
		setBoundsOfSubNode(subNode, newBounds);
	}

	// naive implementation, to be optimized
	inline BoundsTemplate<float> getTotalBounds(int upTo) const {
		assert(upTo > 1 && upTo < BRANCH_FACTOR);
		BoundsTemplate<float> totalBounds = getBoundsOfSubNode(0);
		for(int i = 1; i < upTo; i++) {
			totalBounds = unionOfBounds(totalBounds, getBoundsOfSubNode(i));
		}
		return totalBounds;
	}

	inline void computeAllCombinationCosts(const BoundsTemplate<float>& boundsExtention, float (&costs)[BRANCH_FACTOR]) const {
		for(int i = 0; i < BRANCH_FACTOR; i++) {
			BoundsTemplate<float> subNodeBounds = this->getBoundsOfSubNode(i);
			costs[i] = unionOfBounds(boundsExtention, subNodeBounds).getVolume() - subNodeBounds.getVolume();
		}
	}

	inline int getLowestCombinationCost(const BoundsTemplate<float>& boundsExtention) const {
		float costs[BRANCH_FACTOR];
		this->computeAllCombinationCosts(boundsExtention, costs);
		float bestCost = costs[0];
		int bestIndex = 0;
		for(int i = 1; i < BRANCH_FACTOR; i++) {
			if(costs[i] < bestCost) {
				bestIndex = i;
				bestCost = costs[i];
			}
		}
		return bestIndex;
	}
};

TreeNodeRef::TreeNodeRef(TreeTrunk& trunk, int trunkSize, bool isGroupHead) : ptr(reinterpret_cast<std::intptr_t>(&trunk) | (static_cast<std::intptr_t>(trunkSize) - 1) | (isGroupHead ? BRANCH_FACTOR : 0)) {
	assert(trunkSize >= 2 && trunkSize <= BRANCH_FACTOR); // trunkSize must be between 2-BRANCH_FACTOR
	assert(reinterpret_cast<std::intptr_t>(&trunk) & (BRANCH_FACTOR * sizeof(int64_t) - 1) == 0); // check trunk is aligned correctly
}
BoundsTemplate<float> TreeNodeRef::getTrunkBounds() const {
	assert(isTrunkNode());
	return this->asTrunk().getTotalBounds(this->getTrunkSize());
}
TreeTrunk& TreeNodeRef::asTrunk() const {
	assert(isTrunkNode());
	return *reinterpret_cast<TreeTrunk*>(ptr & ((BRANCH_FACTOR << 1) - 1));
}
void* TreeNodeRef::asObject() const {
	assert(isLeafNode());
	return reinterpret_cast<TreeTrunk*>(ptr & (BRANCH_FACTOR - 1));
}

template<typename CastTo, typename GetObjectBoundsFunc>
inline BoundsTemplate<float> TreeNodeRef::recalculateBoundsRecursive(const GetObjectBoundsFunc& getObjBounds) {
	int sizeData = getSizeData();
	if(sizeData == 0) { // is leaf node
		return getObjBounds(*reinterpret_cast<const CastTo*>(this->asObject()));
	} else {
		int trunkSize = sizeData + 1;

		TreeTrunk* trunk = this->asTrunk();

		for(int i = 0; i < trunkSize; i++) {
			trunk->setBoundsOfSubNode(i, trunk->subNodes[i].recalculateBoundsRecursive(getObjBounds));
		}

		return trunk->getTotalBounds(trunkSize);
	}
}

class BoundsTree {
	TreeTrunk baseTrunk;
	int baseTrunkSize;

	inline TreeTrunk* allocTrunk() const {
		return static_cast<TreeTrunk*>(aligned_malloc(sizeof(TreeTrunk), alignof(TreeTrunk)));
	}
	inline void freeTrunk(TreeTrunk* trunk) const {
		aligned_free(trunk);
	}

	inline static void addToTrunk(TreeTrunk& trunk, int trunkSize, TreeNodeRef&& newNode, const BoundsTemplate<float>& bounds) {
		assert(trunkSize != BRANCH_FACTOR); // trunk not full
		trunk.setSubNode(trunkSize, bounds, std::move(newNode));
	}

	BoundsTemplate<float> recursiveAdd(TreeTrunk& curTrunk, int curTrunkSize, void* newObject, const BoundsTemplate<float>& bounds) {
		if(curTrunkSize == BRANCH_FACTOR) {
			float costs[BRANCH_FACTOR];
			curTrunk.computeAllCombinationCosts(bounds, costs);

			int chosenNode = curTrunk.getLowestCombinationCost(bounds);

			TreeNodeRef& chosen = curTrunk.subNodes[chosenNode];

			// can be inserted into the trunkNode
			if(chosen.isTrunkNode() && !chosen.isGroupHead()) {
				BoundsTemplate<float> newSubBounds = recursiveAdd(chosen.asTrunk(), chosen.getTrunkSize(), newObject, bounds);
				curTrunk.setBoundsOfSubNode(chosenNode, newSubBounds);
			} else {
				TreeTrunk* newTrunk = allocTrunk();

			}


			return curTrunk.getTotalBounds(curTrunkSize);

		} else {
			addToTrunk(curTrunk, curTrunkSize, TreeNodeRef(newObject), bounds);
		}
	}

public:
	BoundsTree() : baseTrunk(), baseTrunkSize(0) {}

	void add(void* newObject, const BoundsTemplate<float>& bounds) {
		
	}
};

};
