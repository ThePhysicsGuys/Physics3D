#pragma once

#include "../math/fix.h"
#include "../math/position.h"
#include "../math/bounds.h"

#include <cstdint>
#include <utility>
#include <cassert>
#include <limits>
#include <array>
#include <optional>

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

	TreeNodeRef(const TreeNodeRef&) = delete;
	TreeNodeRef& operator=(const TreeNodeRef&) = delete;
#ifndef NDEBUG
	TreeNodeRef(TreeNodeRef&& other) noexcept : ptr(other.ptr) { other.ptr = INVALID_REF; }
	TreeNodeRef& operator=(TreeNodeRef&& other) noexcept { this->ptr = other.ptr; other.ptr = INVALID_REF; return *this; }
#else
	TreeNodeRef(TreeNodeRef&& other) = default;
	TreeNodeRef& operator=(TreeNodeRef&& other) = default;
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
private:
	float xMin[BRANCH_FACTOR];
	float yMin[BRANCH_FACTOR];
	float zMin[BRANCH_FACTOR];
	float xMax[BRANCH_FACTOR];
	float yMax[BRANCH_FACTOR];
	float zMax[BRANCH_FACTOR];
public:
	TreeNodeRef subNodes[BRANCH_FACTOR];
	BoundsTemplate<float> getBoundsOfSubNode(int subNode) const;
	BoundsTemplate<float> getTotalBounds(int upTo) const;

	void setBoundsOfSubNode(int subNode, const BoundsTemplate<float>& newBounds);
	void setSubNode(int subNode, TreeNodeRef&& newNode, const BoundsTemplate<float>& newBounds);
	void moveSubNode(int from, int to);

	std::array<bool, BRANCH_FACTOR> getAllContainsBounds(const BoundsTemplate<float>& boundsToContain) const;
	std::array<float, BRANCH_FACTOR> computeAllCombinationCosts(const BoundsTemplate<float>& boundsExtention) const;
	int getLowestCombinationCost(const BoundsTemplate<float>& boundsExtention, int nodeSize) const;
};

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
	TreeTrunk* allocTrunk() const;
	void freeTrunk(TreeTrunk* trunk) const;
};

int addRecursive(TrunkAllocator& allocator, TreeTrunk& curTrunk, int curTrunkSize, TreeNodeRef&& newNode, const BoundsTemplate<float>& bounds);
int removeRecursive(TrunkAllocator& allocator, TreeTrunk& curTrunk, int curTrunkSize, const void* objectToRemove, const BoundsTemplate<float>& bounds);
bool containsObjectRecursive(const TreeTrunk& trunk, int trunkSize, const void* object, const BoundsTemplate<float>& bounds);
const TreeNodeRef* getGroupRecursive(const TreeTrunk& curTrunk, int curTrunkSize, const void* groupRepresentative, const BoundsTemplate<float>& representativeBounds);

/*
 	Expects a function of the form BoundsTemplate<float>(TreeNodeRef& groupNode, const BoundsTemplate<float>& groupNodeBounds)
	Should return the new bounds of the node. 
	The input group and resulting group should be in the normal form of a group:
		if Trunk node => isGroupHead is enabled, no trunk node below has isGroupHead
		or Leaf node
*/
template<typename Func>
inline bool modifyGroupRecursive(TrunkAllocator& allocator, TreeTrunk& curTrunk, int curTrunkSize, const void* groupRepresentative, const BoundsTemplate<float>& groupRepresentativeBounds, const Func& modification) {
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
						BoundsTemplate<float> newTrunkBounds = modification(subNode, curTrunk.getBoundsOfSubNode(i));
						assert(subNode.isGroupHeadOrLeaf());
						curTrunk.setBoundsOfSubNode(i, newTrunkBounds);
						return true;
					}
				} else {
					if(modifyGroupRecursive(allocator, subTrunk, subTrunkSize, groupRepresentative, groupRepresentativeBounds, modification)) {
						curTrunk.setBoundsOfSubNode(i, subTrunk.getTotalBounds(subTrunkSize));
						return true;
					}
				}
			} else {
				if(subNode.asObject() == groupRepresentative) {
					BoundsTemplate<float> newTrunkBounds = modification(subNode, curTrunk.getBoundsOfSubNode(i));
					assert(subNode.isGroupHeadOrLeaf());
					curTrunk.setBoundsOfSubNode(i, newTrunkBounds);
					return true;
				}
			}
		}
	}
	return false;
}
/*
	Expects a function of the form std::optional<BoundsTemplate<float>>(TreeNodeRef& groupNode, const BoundsTemplate<float>& groupNodeBounds)
	Should return the new bounds of the node.
	The input group and resulting group should be in the normal form of a group:
		if Trunk node => isGroupHead is enabled, no trunk node below has isGroupHead
		or Leaf node
*/
template<typename Func>
inline int contractGroupRecursive(TrunkAllocator& allocator, TreeTrunk& curTrunk, int curTrunkSize, const void* groupRepresentative, const BoundsTemplate<float>& groupRepresentativeBounds, const Func& modification) {
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
						std::optional<BoundsTemplate<float>> newTrunkBounds = modification(subNode, curTrunk.getBoundsOfSubNode(i));
						if(newTrunkBounds.has_value()) {
							assert(subNode.isGroupHeadOrLeaf());
							curTrunk.setBoundsOfSubNode(i, newTrunkBounds);
							return curTrunkSize;
						} else {
							curTrunk.moveSubNode(curTrunkSize - 1, i);
							return curTrunkSize - 1;
						}
					}
				} else {
					int newSubSize = contractGroupRecursive(allocator, subTrunk, subTrunkSize, groupRepresentative, groupRepresentativeBounds, modification);
					if(newSubSize != -1) {
						assert(newSubSize >= 1);
						if(newSubSize == 1) {
							curTrunk.moveSubNode(curTrunkSize - 1, i);
							curTrunk.setBoundsOfSubNode(i, subTrunk.getTotalBounds(curTrunkSize - 1));
							return curTrunkSize - 1;
						} else {
							curTrunk.setBoundsOfSubNode(i, subTrunk.getTotalBounds(curTrunkSize));
							return curTrunkSize;
						}
					}
				}
			} else {
				if(subNode.asObject() == groupRepresentative) {
					std::optional<BoundsTemplate<float>> newTrunkBounds = modification(subNode, curTrunk.getBoundsOfSubNode(i));
					if(newTrunkBounds.has_value()) {
						assert(subNode.isGroupHeadOrLeaf());
						curTrunk.setBoundsOfSubNode(i, newTrunkBounds);
						return curTrunkSize;
					} else {
						curTrunk.moveSubNode(curTrunkSize - 1, i);
						return curTrunkSize - 1;
					}
				}
			}
		}
	}
	return false;
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
			TreeTrunk& subTrunk = subNode.asTrunk();
			forEachTrunkRecurse(subTrunk, subNode.getTrunkSize(), func);
			func(subTrunk);
		}
	}
}

class BoundsTree {
	TreeTrunk baseTrunk;
	int baseTrunkSize;
	TrunkAllocator allocator;

public:
	BoundsTree();
	~BoundsTree();

	void add(void* newObject, const BoundsTemplate<float>& bounds);
	void remove(const void* objectToRemove, const BoundsTemplate<float>& bounds);

	void addToGroup(void* newObject, const BoundsTemplate<float>& newObjectBounds, void* groupRepresentative, const BoundsTemplate<float>& groupRepBounds);
	void mergeGroups(const void* groupRepA, const BoundsTemplate<float>& repABounds, const void* groupRepB, const BoundsTemplate<float>& repBBounds);
	
	// the given iterator should return objects of type std::pair<void*, BoundsTemplate<float>>
	template<typename GroupIter, typename GroupIterEnd>
	void transferSplitGroupTo(GroupIter iter, const GroupIterEnd& iterEnd, BoundsTree& destinationTree) {
		if(!(iter != iterEnd)) return; // no items
		std::pair<void*, BoundsTemplate<float>> firstObject = *iter;
		++iter;
		if(!(iter != iterEnd)) { // just one item
			this->baseTrunkSize = removeRecursive(this->allocator, this->baseTrunk, this->baseTrunkSize, firstObject.first, firstObject.second);
			destinationTree.add(firstObject.first, firstObject.second);
		} else {
			int newMainSize = contractGroupRecursive(this->allocator, this->baseTrunk, this->baseTrunkSize, firstObject.first, firstObject.second, [this, &firstObject, &iter, &iterEnd, &destinationTree](TreeNodeRef& group, const BoundsTemplate<float>& groupNodeBounds) {
				if(!group.isTrunkNode()) {
					throw "Attempting to remove more than one object from a single-object group!";
				}
				TreeTrunk& groupTrunk = group.asTrunk();
				int groupTrunkSize = group.getTrunkSize();

				groupTrunkSize = removeRecursive(this->allocator, groupTrunk, groupTrunkSize, firstObject.first, firstObject.second);
				assert(groupTrunkSize != -1); // should have been verified 

				TreeTrunk* newTrunk = destinationTree.allocator.allocTrunk();
				newTrunk->setSubNode(0, TreeNodeRef(firstObject.first), firstObject.second);
				int newTrunkSize = 1;

				do {
					std::pair<void*, BoundsTemplate<float>> currentlyMoving = *iter;
					groupTrunkSize = removeRecursive(this->allocator, groupTrunk, groupTrunkSize, currentlyMoving.first, currentlyMoving.second);
					if(groupTrunkSize == -1) {
						throw "Iterator provided an object which could not be found in this group!";
					}
					newTrunkSize = addRecursive(destinationTree.allocator, groupTrunk, newTrunkSize, TreeNodeRef(currentlyMoving.first), currentlyMoving.second);
					++iter;
				} while(iter != iterEnd);

				destinationTree.baseTrunkSize = addRecursive(destinationTree.allocator, destinationTree.baseTrunk, destinationTree.baseTrunkSize, TreeNodeRef(newTrunk, newTrunkSize, true), newTrunk->getTotalBounds(newTrunkSize));

				if(groupTrunkSize >= 2) {
					return std::optional<BoundsTemplate<float>>(groupTrunk.getTotalBounds(groupTrunkSize));
				} else if(groupTrunkSize == 1) {
					BoundsTemplate<float> resultingBounds = groupTrunk.getBoundsOfSubNode(0);
					group = std::move(groupTrunk.subNodes[0]);
					this->allocator.freeTrunk(&groupTrunk);
					return std::optional<BoundsTemplate<float>>(resultingBounds);
				} else {
					this->allocator.freeTrunk(&groupTrunk);
					return std::optional<BoundsTemplate<float>>();
				}
			});
			if(newMainSize == -1) {
				throw "Could not find group!";
			}
			this->baseTrunkSize = newMainSize;
		}
	}
	void transferGroupTo(const void* groupRep, const BoundsTemplate<float>& groupRepBounds, BoundsTree& destinationTree);

	// the given iterator should return objects of type std::pair<void*, BoundsTemplate<float>>
	template<typename GroupIter, typename GroupIterEnd>
	void splitGroup(GroupIter iter, const GroupIterEnd& iterEnd, BoundsTree& destinationTree) {
		this->transferSplitGroupTo(iter, iterEnd, *this);
	}

	bool contains(const void* object, const BoundsTemplate<float>& bounds) const;
	bool groupContains(const void* groupRep, const BoundsTemplate<float>& groupRepBounds, const void* object, const BoundsTemplate<float>& bounds) const;
	size_t size() const;
	size_t groupSize(const void* groupRep, const BoundsTemplate<float>& groupRepBounds) const;

	inline bool isEmpty() const {
		return this->baseTrunkSize == 0;
	}

	void clear();

	// expects a function of the form void(void* object, const BoundsTemplate<float>& bounds)
	template<typename Func>
	inline void forEach(const Func& func) const {
		forEachRecurse(this->baseTrunk, this->baseTrunkSize, func);
	}

	// expects a function of the form void(void* object, const BoundsTemplate<float>& bounds)
	template<typename Func>
	inline void forEachInGroup(const void* groupRepresentative, const BoundsTemplate<float>& groupRepBounds, const Func& func) const {
		const TreeNodeRef* group = getGroupRecursive(this->baseTrunk, this->baseTrunkSize, groupRepresentative, groupRepBounds);
		if(group == nullptr) {
			throw "Group not found!";
		}
		if(group->isTrunkNode()) {
			forEachRecurse(group->asTrunk(), group->getTrunkSize(), func);
		} else {
			func(group->asObject());
		}
	}
};

template<typename BoundableIter>
class BoundableIteratorAdapter {
	BoundableIter iter;

	std::pair<void*, BoundsTemplate<float>> operator*() const {
		auto& item = *iter;
		return std::pair<void*, BoundsTemplate<float>>(static_cast<void*>(&item), item.getBounds());
	}
	BoundableIteratorAdapter& operator++() {
		++iter;
		return *this;
	}
	template<typename BoundableIterEnd>
	bool operator!=(const BoundableIterEnd& end) const {
		return this->iter != end;
	}
};

struct BasicBounded {
	BoundsTemplate<float> bounds;

	BoundsTemplate<float> getBounds() const { return bounds; }
};

};
