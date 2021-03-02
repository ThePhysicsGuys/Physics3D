#pragma once

#include "../math/fix.h"
#include "../math/position.h"
#include "../math/bounds.h"
#include "aligned_alloc.h"

#include <cstdint>
#include <utility>
#include <cassert>
#include <limits>
#include <array>

namespace P3D::NewBoundsTree {

constexpr int BRANCH_FACTOR = 8;
static_assert((BRANCH_FACTOR & (BRANCH_FACTOR - 1)) == 0, "Branch factor must be power of 2");

class TreeTrunk;
class BoundsTree;
class TreeNodeRef;

class TreeNodeRef {
	friend class TreeTrunk;
	friend class BoundsTree;

	static constexpr std::uintptr_t SIZE_DATA_MASK = BRANCH_FACTOR - 1;
	static constexpr std::uintptr_t GROUP_HEAD_MASK = BRANCH_FACTOR;
	static constexpr std::uintptr_t PTR_MASK = ~(SIZE_DATA_MASK | GROUP_HEAD_MASK);
	static constexpr std::uintptr_t INVALID_REF = 0xADADADADADADADAD;


	/* encoding:
		0b...pppppgsss

		(this is if BRANCH_FACTOR == 8)
		Last 3 bits specify type: 
		0b000: leaf node -> ptr points to object
		else : trunk node -> ptr points to TreeTrunk
			g bit specifies 'isGroupHead'
			s bits specify size of this trunk node - 1. 0b111 is a trunknode of size 8
	*/
	std::uintptr_t ptr;

	inline int getSizeData() const {
		return static_cast<int>(ptr & SIZE_DATA_MASK);
	}

	inline BoundsTemplate<float> getTrunkBounds() const;

public:
#ifndef NDEBUG
	TreeNodeRef() noexcept : ptr(INVALID_REF) {}
#else
	TreeNodeRef() = default;
#endif
	inline explicit TreeNodeRef(TreeTrunk* trunk, int trunkSize, bool isGroupHead) noexcept;
	inline explicit TreeNodeRef(void* ptr) noexcept : ptr(reinterpret_cast<std::uintptr_t>(ptr)) {
		assert((this->ptr & SIZE_DATA_MASK) == 0);
	}

#ifndef NDEBUG
	TreeNodeRef(const TreeNodeRef&) = default;
	TreeNodeRef& operator=(const TreeNodeRef&) = default;
	TreeNodeRef(TreeNodeRef&& other) noexcept : ptr(other.ptr) { other.ptr = INVALID_REF; }
	TreeNodeRef& operator=(TreeNodeRef&& other) noexcept { this->ptr = other.ptr; other.ptr = INVALID_REF; return *this; }
#endif

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
	inline void setTrunkSize(int newSize) {
		assert(isTrunkNode());
		assert(newSize >= 2 && newSize <= BRANCH_FACTOR);
		this->ptr = this->ptr & ~SIZE_DATA_MASK | (newSize - 1);
	}
	inline void makeGroupHead() {
		assert(isTrunkNode());
		this->ptr |= GROUP_HEAD_MASK;
	}
	inline void removeGroupHead() {
		assert(isTrunkNode());
		this->ptr &= ~GROUP_HEAD_MASK;
	}
	inline bool isGroupHead() const {
		assert(isTrunkNode());
		return (ptr & GROUP_HEAD_MASK) != 0;
	}
	inline bool isGroupHeadOrLeaf() const {
		if(isTrunkNode()) {
			return (ptr & GROUP_HEAD_MASK) != 0;
		} else {
			return true;
		}
	}
	inline const TreeTrunk& asTrunk() const;
	inline TreeTrunk& asTrunk();
	inline void* asObject() const;

	// expects a function of the form BoundsTemplate<float>(const CastTo&)
	template<typename CastTo, typename GetObjectBoundsFunc>
	inline BoundsTemplate<float> recalculateBoundsRecursive(const GetObjectBoundsFunc& getObjBounds);
};

class alignas(64) TreeTrunk {
	float xMin[BRANCH_FACTOR];
	float yMin[BRANCH_FACTOR];
	float zMin[BRANCH_FACTOR];
	float xMax[BRANCH_FACTOR];
	float yMax[BRANCH_FACTOR];
	float zMax[BRANCH_FACTOR];
public:
	TreeNodeRef subNodes[BRANCH_FACTOR];
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

	inline void setBoundsOfSubNode(int subNode, const BoundsTemplate<float>& newBounds) {
		assert(subNode >= 0 && subNode < BRANCH_FACTOR);
		xMin[subNode] = newBounds.min.x;
		yMin[subNode] = newBounds.min.y;
		zMin[subNode] = newBounds.min.z;
		xMax[subNode] = newBounds.max.x;
		yMax[subNode] = newBounds.max.y;
		zMax[subNode] = newBounds.max.z;
	}

	inline void setSubNode(int subNode, const BoundsTemplate<float>& newBounds, TreeNodeRef&& newNode) {
		assert(subNode >= 0 && subNode < BRANCH_FACTOR);
		subNodes[subNode] = std::move(newNode);
		setBoundsOfSubNode(subNode, newBounds);
	}

	// naive implementation, to be optimized
	inline BoundsTemplate<float> getTotalBounds(int upTo) const {
		assert(upTo > 1 && upTo <= BRANCH_FACTOR);
		BoundsTemplate<float> totalBounds = getBoundsOfSubNode(0);
		for(int i = 1; i < upTo; i++) {
			totalBounds = unionOfBounds(totalBounds, getBoundsOfSubNode(i));
		}
		return totalBounds;
	}

	inline std::array<bool, BRANCH_FACTOR> getAllContainsBounds(const BoundsTemplate<float>& boundsToContain) const {
		std::array<bool, BRANCH_FACTOR> contained;
		for(int i = 0; i < BRANCH_FACTOR; i++) {
			BoundsTemplate<float> subNodeBounds = this->getBoundsOfSubNode(i);
			contained[i] = subNodeBounds.contains(boundsToContain);
		}
		return contained;
	}

	inline std::array<float, BRANCH_FACTOR> computeAllCombinationCosts(const BoundsTemplate<float>& boundsExtention) const {
		std::array<float, BRANCH_FACTOR> costs;
		for(int i = 0; i < BRANCH_FACTOR; i++) {
			BoundsTemplate<float> subNodeBounds = this->getBoundsOfSubNode(i);
			costs[i] = unionOfBounds(boundsExtention, subNodeBounds).getVolume() - subNodeBounds.getVolume();
		}
		return costs;
	}

	inline int getLowestCombinationCost(const BoundsTemplate<float>& boundsExtention, int nodeSize) const {
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

	inline void moveSubNode(int from, int to) {
		this->setSubNode(to, this->getBoundsOfSubNode(from), std::move(this->subNodes[from]));
	}
};

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

class TrunkAllocator {

public:
	inline TreeTrunk* allocTrunk() const {
		return static_cast<TreeTrunk*>(aligned_malloc(sizeof(TreeTrunk), alignof(TreeTrunk)));
	}
	inline void freeTrunk(TreeTrunk* trunk) const {
		aligned_free(trunk);
	}
};

// returns the new size of this node, to be applied to the caller
inline [[nodiscard]] int addRecursive(TrunkAllocator& allocator, TreeTrunk& curTrunk, int curTrunkSize, void* newObject, const BoundsTemplate<float>& bounds) {
	assert(curTrunkSize >= 0 && curTrunkSize <= BRANCH_FACTOR);
	if(curTrunkSize == BRANCH_FACTOR) {
		int chosenNode = curTrunk.getLowestCombinationCost(bounds, curTrunkSize);

		TreeNodeRef& chosen = curTrunk.subNodes[chosenNode];
		BoundsTemplate<float> oldSubNodeBounds = curTrunk.getBoundsOfSubNode(chosenNode);

		// can be inserted into the trunkNode
		if(chosen.isTrunkNode() && !chosen.isGroupHead()) {
			int newSize = addRecursive(allocator, chosen.asTrunk(), chosen.getTrunkSize(), newObject, bounds);
			chosen.setTrunkSize(newSize);
		} else {
			TreeTrunk* newTrunk = allocator.allocTrunk();
			newTrunk->setSubNode(0, oldSubNodeBounds, std::move(chosen));
			newTrunk->setSubNode(1, bounds, TreeNodeRef(newObject));

			chosen = TreeNodeRef(newTrunk, 2, false);
		}
		curTrunk.setBoundsOfSubNode(chosenNode, unionOfBounds(oldSubNodeBounds, bounds));
		return BRANCH_FACTOR;
	} else {
		curTrunk.setSubNode(curTrunkSize, bounds, TreeNodeRef(newObject));
		return curTrunkSize + 1;
	}
}

inline [[nodiscard]] bool containsObjectRecursive(const TreeTrunk& trunk, int trunkSize, const void* object, const BoundsTemplate<float>& bounds) {
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

template<typename Func>
/*
 	Expects a function of the form BoundsTemplate<float>(TrunkAllocator& allocator, TreeNodeRef& groupNode, BoundsTemplate<float>& groupNodeBounds)
	Should return the new bounds of the node. 
	The input group and resulting group should be in the normal form of a group:
		if Trunk node => isGroupHead is enabled, no trunk node below has isGroupHead
		or Leaf node
*/
inline [[nodiscard]] bool modifyGroupRecursive(TrunkAllocator& allocator, TreeTrunk& curTrunk, int curTrunkSize, const void* groupRepresentative, const BoundsTemplate<float>& groupRepresentativeBounds, const Func& modification) {
	assert(curTrunkSize >= 0 && curTrunkSize <= BRANCH_FACTOR);
	std::array<bool, BRANCH_FACTOR> couldContain = curTrunk.getAllContainsBounds(groupRepresentativeBounds);
	for(int i = 0; i < curTrunkSize; i++) {
		if(couldContain[i]) {
			TreeNodeRef& subNode = curTrunk.subNodes[i];
			if(subNode.isTrunkNode()) {
				TreeTrunk& subTrunk = subNode.asTrunk();
				int subTrunkSize = subNode.getTrunkSize();
				if(subNode.isGroupHead()) {
					if(containsObjectRecursive(subTrunk, subTrunkSize, groupRepresentative, groupRepresentativeBounds)) {
						BoundsTemplate<float> newTrunkBounds = modification(allocator, subNode, curTrunk.getBoundsOfSubNode(i));
						assert(subNode.isGroupHeadOrLeaf());
						curTrunk.setBoundsOfSubNode(i, newTrunkBounds);
						return true;
					}
				} else {
					if(modifyGroupRecursive(allocator, subTrunk, subTrunkSize, groupRepresentative, groupRepresentativeBounds, modification)) {
						return true;
					}
				}
			} else {
				if(subNode.asObject() == groupRepresentative) {
					BoundsTemplate<float> newTrunkBounds = modification(allocator, subNode, curTrunk.getBoundsOfSubNode(i));
					assert(subNode.isGroupHeadOrLeaf());
					curTrunk.setBoundsOfSubNode(i, newTrunkBounds);
					return true;
				}
			}
		}
	}
	return false;
}

// returns new trunkSize if removed, -1 if not removed
inline [[nodiscard]] int removeRecursive(TrunkAllocator& allocator, TreeTrunk& curTrunk, int curTrunkSize, const void* objectToRemove, const BoundsTemplate<float>& bounds) {
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
						curTrunk.setSubNode(i, subNodeTrunk.getBoundsOfSubNode(0), std::move(subNodeTrunk.subNodes[0]));
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
inline [[nodiscard]] TreeGrab grabGroupRecursive(TrunkAllocator& allocator, TreeTrunk& curTrunk, int curTrunkSize, const void* groupRepresentative, const BoundsTemplate<float>& representativeBounds) {
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
								curTrunk.setSubNode(i, subNodeTrunk.getBoundsOfSubNode(0), std::move(subNodeTrunk.subNodes[0]));
								allocator.freeTrunk(&subNodeTrunk);
							} else {
								curTrunk.setBoundsOfSubNode(i, subNodeTrunk.getTotalBounds(newSubNodeTrunkSize));
								subNode.setTrunkSize(newSubNodeTrunkSize);
							}

							return TreeGrab(curTrunkSize, std::move(recursiveResult.nodeRef), recursiveResult.nodeBounds);
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

// expects a function of the form void(void* object, const BoundsTemplate<float>& bounds)
template<typename Func>
inline void forEachRecurseWithBounds(const TreeTrunk& curTrunk, int curTrunkSize, const Func& func) {
	for(int i = 0; i < curTrunkSize; i++) {
		const TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			forEachRecurseWithBounds(subNode.asTrunk(), subNode.getTrunkSize(), func);
		} else {
			func(subNode.asObject(), curTrunk.getBoundsOfSubNode(i));
		}
	}
}

// expects a function of the form void(void* object, const BoundsTemplate<float>& bounds)
template<typename Func>
inline void forEachRecurse(const TreeTrunk& curTrunk, int curTrunkSize, const Func& func) {
	for(int i = 0; i < curTrunkSize; i++) {
		const TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			forEachRecurse(subNode.asTrunk(), subNode.getTrunkSize(), func);
		} else {
			func(subNode.asObject());
		}
	}
}

// expects a function of the form void(TrunkNode& trunk)
// runs first for leaf trunks
template<typename Func>
inline void forEachTrunkRecurse(TreeTrunk& curTrunk, int curTrunkSize, const Func& func) {
	for(int i = 0; i < curTrunkSize; i++) {
		TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			forEachTrunkRecurse(subNode.asTrunk(), subNode.getTrunkSize(), func);
		}
	}
	func(curTrunk);
}

// returns true if the group that is inserted into is found
// deletes all trunknodes of the destroyed group with the provided allocator
inline bool insertGroupIntoGroup(TrunkAllocator& alloc, TreeTrunk& baseTrunk, int baseTrunkSize, const void* groupToAddToRep, const BoundsTemplate<float>& groupToAddToRepBounds, TreeNodeRef&& groupToDestroy, const BoundsTemplate<float>& groupToDestroyBounds) {
	bool groupWasFound = modifyGroupRecursive(alloc, baseTrunk, baseTrunkSize, groupToAddToRep, groupToAddToRepBounds, [&groupToDestroy, &groupToDestroyBounds](TrunkAllocator& alloc, TreeNodeRef& group, const BoundsTemplate<float>& groupBounds) {
		TreeTrunk* trunk;
		int trunkSize;
		if(group.isLeafNode()) {
			trunk = alloc.allocTrunk();
			trunk->setSubNode(0, groupBounds, std::move(group));
		} else {
			trunk = &group.asTrunk();
			trunkSize = group.getTrunkSize();
		}

		if(groupToDestroy.isTrunkNode()) {
			forEachRecurseWithBounds(groupToDestroy.asTrunk(), groupToDestroy.getTrunkSize(), [&](void* object, const BoundsTemplate<float>& bounds) {
				trunkSize = addRecursive(alloc, *trunk, trunkSize, object, bounds);
			});
		} else {
			trunkSize = addRecursive(alloc, *trunk, trunkSize, groupToDestroy.asObject(), groupToDestroyBounds);
		}

		group = TreeNodeRef(trunk, trunkSize, true);

		if(groupToDestroy.isTrunkNode()) {
			forEachTrunkRecurse(groupToDestroy.asTrunk(), groupToDestroy.getTrunkSize(), [&alloc](TreeTrunk& t) {
				alloc.freeTrunk(&t);
			});
		}

		return trunk->getTotalBounds(trunkSize);
	});
	return groupWasFound;
}

class BoundsTree {
	TreeTrunk baseTrunk;
	int baseTrunkSize;
	TrunkAllocator allocator;

public:
	BoundsTree() : baseTrunk(), baseTrunkSize(0) {}

	inline void add(void* newObject, const BoundsTemplate<float>& bounds) {
		this->baseTrunkSize = addRecursive(allocator, baseTrunk, baseTrunkSize, newObject, bounds);
	}
	inline void addToGroup(void* newObject, const BoundsTemplate<float>& newObjectBounds, void* groupRepresentative, const BoundsTemplate<float>& groupRepBounds) {
		bool foundGroup = modifyGroupRecursive(allocator, baseTrunk, baseTrunkSize, groupRepresentative, groupRepBounds, [newObject, &newObjectBounds](TrunkAllocator& allocator, TreeNodeRef& groupNode, const BoundsTemplate<float>& groupNodeBounds) {
			assert(groupNode.isGroupHeadOrLeaf());
			if(groupNode.isTrunkNode()) {
				TreeTrunk& groupTrunk = groupNode.asTrunk();
				int resultingSize = addRecursive(allocator, groupTrunk, groupNode.getTrunkSize(), newObject, newObjectBounds);
				groupNode.setTrunkSize(resultingSize);
				return groupTrunk.getTotalBounds(resultingSize);
			} else {
				TreeTrunk* newTrunkNode = allocator.allocTrunk();
				newTrunkNode->setSubNode(0, groupNodeBounds, std::move(groupNode));
				newTrunkNode->setSubNode(1, newObjectBounds, TreeNodeRef(newObject));
				groupNode = TreeNodeRef(newTrunkNode, 2, true);
				return newTrunkNode->getTotalBounds(2);
			}
		});
		if(!foundGroup) {
			throw "Group was not found!";
		}
	}
	inline void mergeGroups(const void* groupRepA, const BoundsTemplate<float>& repABounds, const void* groupRepB, const BoundsTemplate<float>& repBBounds) {
		TreeGrab grabbed = grabGroupRecursive(this->allocator, this->baseTrunk, this->baseTrunkSize, groupRepA, repABounds);
		if(grabbed.resultingGroupSize == -1) {
			throw "groupRepA not found!";
		}
		this->baseTrunkSize = grabbed.resultingGroupSize;

		bool groupBWasFound = insertGroupIntoGroup(this->allocator, this->baseTrunk, this->baseTrunkSize, groupRepB, repBBounds, std::move(grabbed.nodeRef), grabbed.nodeBounds);
		
		if(!groupBWasFound) {
			throw "groupRepB not found!";
		}
	}
	inline void remove(const void* objectToRemove, const BoundsTemplate<float>& bounds) {
		int resultingBaseSize = removeRecursive(allocator, baseTrunk, baseTrunkSize, objectToRemove, bounds);
		if(resultingBaseSize != -1) {
			this->baseTrunkSize = resultingBaseSize;
		} else {
			throw "Object not found in tree!";
		}
	}
	inline bool contains(const void* object, const BoundsTemplate<float>& bounds) const {
		return containsObjectRecursive(baseTrunk, baseTrunkSize, object, bounds);
	}

	// expects a function of the form void(void* object, const BoundsTemplate<float>& bounds)
	template<typename Func>
	inline void forEach(const Func& func) const {
		forEachRecurse(this->baseTrunk, this->baseTrunkSize, func);
	}

	inline size_t size() const {
		size_t total = 0;
		this->forEach([&total](const void* item) {
			total++;
		});
		return total;
	}

	inline bool isEmpty() const {
		return this->baseTrunkSize == 0;
	}
};

struct BasicBounded {
	BoundsTemplate<float> bounds;

	BoundsTemplate<float> getBounds() const { return bounds; }
};

};
