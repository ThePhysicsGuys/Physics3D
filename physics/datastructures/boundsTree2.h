#pragma once

#include "../math/fix.h"
#include "../math/position.h"
#include "../math/bounds.h"
#include "iteratorEnd.h"

#include <cstdint>
#include <utility>
#include <cassert>
#include <limits>
#include <array>
#include <optional>
#include <iostream>
#include <stack>

namespace P3D::NewBoundsTree {

constexpr int BRANCH_FACTOR = 8;
static_assert((BRANCH_FACTOR & (BRANCH_FACTOR - 1)) == 0, "Branch factor must be power of 2");

class TreeTrunk;

inline float computeCost(const BoundsTemplate<float>& bounds) {
	Vec3f d = bounds.getDiagonal();
	return d.x + d.y + d.z;
}


class TreeNodeRef {
	friend class TreeTrunk;

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
		assert(this->ptr != INVALID_REF);
		return getSizeData() == 0;
	}
	inline bool isTrunkNode() const {
		assert(this->ptr != INVALID_REF);
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
	inline void setObject(void* newObject) {
		assert(isLeafNode());
		this->ptr = reinterpret_cast<std::uintptr_t>(newObject);
		assert((this->ptr & SIZE_DATA_MASK) == 0);
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

struct TrunkSIMDHelperFallback;

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
	void setBoundsOfSubNode(int subNode, const BoundsTemplate<float>& newBounds);
	void setSubNode(int subNode, TreeNodeRef&& newNode, const BoundsTemplate<float>& newBounds);
	void moveSubNode(int from, int to);
};

TreeNodeRef::TreeNodeRef(TreeTrunk* trunk, int trunkSize, bool isGroupHead) noexcept : ptr(reinterpret_cast<std::uintptr_t>(trunk) | (static_cast<std::uintptr_t>(trunkSize) - 1) | (isGroupHead ? BRANCH_FACTOR : 0)) {
	assert(trunkSize >= 2 && trunkSize <= BRANCH_FACTOR); // trunkSize must be between 2-BRANCH_FACTOR
	assert((reinterpret_cast<std::uintptr_t>(trunk) & (BRANCH_FACTOR * sizeof(int64_t) - 1)) == 0); // check trunk is aligned correctly
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

typedef std::array<std::array<bool, BRANCH_FACTOR>, BRANCH_FACTOR> OverlapMatrix;

struct TrunkSIMDHelperFallback {
	static BoundsTemplate<float> getTotalBounds(const TreeTrunk& trunk, int upTo);
	static std::array<bool, BRANCH_FACTOR> getAllContainsBounds(const TreeTrunk& trunk, const BoundsTemplate<float>& boundsToContain);
	static std::array<float, BRANCH_FACTOR> computeAllCombinationCosts(const TreeTrunk& trunk, const BoundsTemplate<float>& boundsExtention);
	static int getLowestCombinationCost(const TreeTrunk& trunk, const BoundsTemplate<float>& boundsExtention, int nodeSize);
	static std::array<bool, BRANCH_FACTOR> computeOverlapsWith(const TreeTrunk& trunk, int trunkSize, const BoundsTemplate<float>& bounds);
	// indexed result[a][b]
	static std::array<std::array<bool, BRANCH_FACTOR>, BRANCH_FACTOR> computeBoundsOverlapMatrix(const TreeTrunk& trunkA, int trunkASize, const TreeTrunk& trunkB, int trunkBSize);
	// indexed result[i][j] with j >= i+1
	static std::array<std::array<bool, BRANCH_FACTOR>, BRANCH_FACTOR> computeInternalBoundsOverlapMatrix(const TreeTrunk& trunk, int trunkSize);
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

		return TrunkSIMDHelperFallback::getTotalBounds(*trunk, trunkSize);
	}
}

class TrunkAllocator {
	size_t allocationCount = 0;
public:
	~TrunkAllocator();
	TreeTrunk* allocTrunk();
	void freeTrunk(TreeTrunk* trunk);
	void freeAllTrunks(TreeTrunk& baseTrunk, int baseTrunkSize);
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
	std::array<bool, BRANCH_FACTOR> couldContain = TrunkSIMDHelperFallback::getAllContainsBounds(curTrunk, groupRepresentativeBounds);
	for(int i = 0; i < curTrunkSize; i++) {
		if(!couldContain[i]) continue;

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
					curTrunk.setBoundsOfSubNode(i, TrunkSIMDHelperFallback::getTotalBounds(subTrunk, subTrunkSize));
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
	std::array<bool, BRANCH_FACTOR> couldContain = TrunkSIMDHelperFallback::getAllContainsBounds(curTrunk, groupRepresentativeBounds);
	for(int i = 0; i < curTrunkSize; i++) {
		if(!couldContain[i]) continue;

		TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			TreeTrunk& subTrunk = subNode.asTrunk();
			int subTrunkSize = subNode.getTrunkSize();
			if(subNode.isGroupHead()) {
				if(containsObjectRecursive(subTrunk, subTrunkSize, groupRepresentative, groupRepresentativeBounds)) {
					std::optional<BoundsTemplate<float>> newTrunkBounds = modification(subNode, curTrunk.getBoundsOfSubNode(i));
					if(newTrunkBounds.has_value()) {
						assert(subNode.isGroupHeadOrLeaf());
						curTrunk.setBoundsOfSubNode(i, newTrunkBounds.value());
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
						curTrunk.setSubNode(i, std::move(subTrunk.subNodes[0]), subTrunk.getBoundsOfSubNode(0));
						allocator.freeTrunk(&subTrunk);
					} else {
						subNode.setTrunkSize(newSubSize);
						curTrunk.setBoundsOfSubNode(i, TrunkSIMDHelperFallback::getTotalBounds(subTrunk, newSubSize));
					}
					return curTrunkSize;
				}
			}
		} else {
			if(subNode.asObject() == groupRepresentative) {
				std::optional<BoundsTemplate<float>> newTrunkBounds = modification(subNode, curTrunk.getBoundsOfSubNode(i));
				if(newTrunkBounds.has_value()) {
					assert(subNode.isGroupHeadOrLeaf());
					curTrunk.setBoundsOfSubNode(i, newTrunkBounds.value());
					return curTrunkSize;
				} else {
					curTrunk.moveSubNode(curTrunkSize - 1, i);
					return curTrunkSize - 1;
				}
			}
		}
	}
	return -1;
}

// expects a function of the form void(Boundable& object)
template<typename Boundable, typename Func>
inline void forEachRecurse(const TreeTrunk& curTrunk, int curTrunkSize, const Func& func) {
	for(int i = 0; i < curTrunkSize; i++) {
		const TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			forEachRecurse<Boundable, Func>(subNode.asTrunk(), subNode.getTrunkSize(), func);
		} else {
			func(*static_cast<Boundable*>(subNode.asObject()));
		}
	}
}

// expects a function of the form void(Boundable*, Boundable*)
// Calls the given function for each pair of leaf nodes from the two trunks 
template<typename Boundable, typename SIMDHelper, typename Func>
void forEachColissionWithRecursive(Boundable* obj, const BoundsTemplate<float>& objBounds, const TreeTrunk& trunk, int trunkSize, const Func& func) {
	std::array<bool, BRANCH_FACTOR> collidesWith = SIMDHelper::computeOverlapsWith(trunk, trunkSize, objBounds);

	for(int i = 0; i < trunkSize; i++) {
		if(!collidesWith[i]) continue;

		const TreeNodeRef& subNode = trunk.subNodes[i];

		if(subNode.isTrunkNode()) {
			forEachColissionWithRecursive<Boundable, SIMDHelper, Func>(obj, objBounds, subNode.asTrunk(), subNode.getTrunkSize(), func);
		} else {
			func(obj, static_cast<Boundable*>(subNode.asObject()));
		}
	}
}

// expects a function of the form void(Boundable*, Boundable*)
// Calls the given function for each pair of leaf nodes from the two trunks 
template<typename Boundable, typename SIMDHelper, typename Func>
void forEachColissionWithRecursive(const TreeTrunk& trunk, int trunkSize, Boundable* obj, const BoundsTemplate<float>& objBounds, const Func& func) {
	std::array<bool, BRANCH_FACTOR> collidesWith = SIMDHelper::computeOverlapsWith(trunk, trunkSize, objBounds);

	for(int i = 0; i < trunkSize; i++) {
		if(!collidesWith[i]) continue;

		const TreeNodeRef& subNode = trunk.subNodes[i];

		if(subNode.isTrunkNode()) {
			forEachColissionWithRecursive<Boundable, SIMDHelper, Func>(subNode.asTrunk(), subNode.getTrunkSize(), obj, objBounds, func);
		} else {
			func(static_cast<Boundable*>(subNode.asObject()), obj);
		}
	}
}

// expects a function of the form void(Boundable*, Boundable*)
// Calls the given function for each pair of leaf nodes from the two trunks 
template<typename Boundable, typename SIMDHelper, typename Func>
void forEachColissionBetweenRecursive(const TreeTrunk& trunkA, int trunkASize, const TreeTrunk& trunkB, int trunkBSize, const Func& func) {
	OverlapMatrix overlapBetween = SIMDHelper::computeBoundsOverlapMatrix(trunkA, trunkASize, trunkB, trunkBSize);

	for(int a = 0; a < trunkASize; a++) {
		const TreeNodeRef& aNode = trunkA.subNodes[a];
		bool aIsTrunk = aNode.isTrunkNode();
		for(int b = 0; b < trunkBSize; b++) {
			if(!overlapBetween[a][b]) continue;

			const TreeNodeRef& bNode = trunkB.subNodes[b];
			bool bIsTrunk = bNode.isTrunkNode();
			
			if(aIsTrunk) {
				if(bIsTrunk) {
					// both trunk nodes
					// split both
					forEachColissionBetweenRecursive<Boundable, SIMDHelper, Func>(aNode.asTrunk(), aNode.getTrunkSize(), bNode.asTrunk(), bNode.getTrunkSize(), func);
				} else {
					// a is trunk, b is not
					// split a
					forEachColissionWithRecursive<Boundable, SIMDHelper, Func>(aNode.asTrunk(), aNode.getTrunkSize(), static_cast<Boundable*>(bNode.asObject()), trunkB.getBoundsOfSubNode(b), func);
				}
			} else {
				if(bIsTrunk) {
					// b is trunk, a is not
					// split b
					forEachColissionWithRecursive<Boundable, SIMDHelper, Func>(static_cast<Boundable*>(aNode.asObject()), trunkA.getBoundsOfSubNode(a), bNode.asTrunk(), bNode.getTrunkSize(), func);
				} else {
					// both leaf nodes
					func(static_cast<Boundable*>(aNode.asObject()), static_cast<Boundable*>(bNode.asObject()));
				}
			}
		}
	}
}

// expects a function of the form void(Boundable*, Boundable*)
// Calls the given function for each pair of leaf nodes that are not in the same group and have overlapping bounds
template<typename Boundable, typename SIMDHelper, typename Func>
void forEachColissionInternalRecursive(const TreeTrunk& curTrunk, int curTrunkSize, const Func& func) {
	OverlapMatrix internalOverlap = SIMDHelper::computeInternalBoundsOverlapMatrix(curTrunk, curTrunkSize);

	for(int a = 0; a < curTrunkSize; a++) {
		const TreeNodeRef& aNode = curTrunk.subNodes[a];
		bool aIsTrunk = aNode.isTrunkNode();
		for(int b = a+1; b < curTrunkSize; b++) {
			if(!internalOverlap[a][b]) continue;

			const TreeNodeRef& bNode = curTrunk.subNodes[b];
			bool bIsTrunk = bNode.isTrunkNode();

			if(aIsTrunk) {
				if(bIsTrunk) {
					// both trunk nodes
					// split both
					forEachColissionBetweenRecursive<Boundable, SIMDHelper, Func>(aNode.asTrunk(), aNode.getTrunkSize(), bNode.asTrunk(), bNode.getTrunkSize(), func);
				} else {
					// a is trunk, b is not
					// split a
					forEachColissionWithRecursive<Boundable, SIMDHelper, Func>(aNode.asTrunk(), aNode.getTrunkSize(), static_cast<Boundable*>(bNode.asObject()), curTrunk.getBoundsOfSubNode(b), func);
				}
			} else {
				if(bIsTrunk) {
					// b is trunk, a is not
					// split b
					forEachColissionWithRecursive<Boundable, SIMDHelper, Func>(static_cast<Boundable*>(aNode.asObject()), curTrunk.getBoundsOfSubNode(a), bNode.asTrunk(), bNode.getTrunkSize(), func);
				} else {
					// both leaf nodes
					func(static_cast<Boundable*>(aNode.asObject()), static_cast<Boundable*>(bNode.asObject()));
				}
			}
		}
	}

	for(int i = 0; i < curTrunkSize; i++) {
		const TreeNodeRef& subNode = curTrunk.subNodes[i];

		if(subNode.isTrunkNode() && !subNode.isGroupHead()) {
			forEachColissionInternalRecursive<Boundable, SIMDHelper, Func>(subNode.asTrunk(), subNode.getTrunkSize(), func);
		}
	}
}

class BoundsTreeIteratorPrototype {
	struct StackElement {
		const TreeTrunk* trunk;
		int trunkSize;
		int curIndex;
	};

	std::stack<StackElement> trunkStack;

public:
	BoundsTreeIteratorPrototype() = default;
	BoundsTreeIteratorPrototype(const TreeTrunk& baseTrunk, int baseTrunkSize) : trunkStack() {
		if(baseTrunkSize == 0) return;
		trunkStack.push(StackElement{&baseTrunk, baseTrunkSize, 0});
		while(trunkStack.top().trunk->subNodes[0].isTrunkNode()) {
			const TreeNodeRef& nextNode = trunkStack.top().trunk->subNodes[0];
			trunkStack.push(StackElement{&nextNode.asTrunk(), nextNode.getTrunkSize(), 0});
		}
	}

	void* operator*() const {
		const StackElement& top = trunkStack.top();
		return top.trunk->subNodes[top.curIndex].asObject();
	}

	BoundsTreeIteratorPrototype& operator++() {
		StackElement& top = trunkStack.top();
		top.curIndex++;
		if(top.curIndex < top.trunkSize) {
			const TreeNodeRef& nextNode = trunkStack.top().trunk->subNodes[top.curIndex];
			if(nextNode.isTrunkNode()) {
				// rise until hitting non trunk node
				trunkStack.push(StackElement{&nextNode.asTrunk(), nextNode.getTrunkSize(), 0});
				while(trunkStack.top().trunk->subNodes[0].isTrunkNode()) {
					const TreeNodeRef& nextNode = trunkStack.top().trunk->subNodes[0];
					trunkStack.push(StackElement{&nextNode.asTrunk(), nextNode.getTrunkSize(), 0});
				}
			}
		} else {
			// drop down until next available
			do {
				trunkStack.pop();
				if(trunkStack.size() == 0) break; // iteration done
				StackElement& top = trunkStack.top();
				top.curIndex++;
				assert(top.curIndex <= top.trunkSize);
			} while(top.curIndex == top.trunkSize); // keep popping
		}
		return *this;
	}

	bool operator!=(IteratorEnd) const {
		return trunkStack.size() != 0;
	}
};

class BoundsTreePrototype {
	TreeTrunk baseTrunk;
	int baseTrunkSize;
	TrunkAllocator allocator;

	template<typename Boundable>
	friend class BoundsTree;

public:
	BoundsTreePrototype();
	~BoundsTreePrototype();

	inline BoundsTreePrototype(BoundsTreePrototype&& other) noexcept : baseTrunk(std::move(other.baseTrunk)), baseTrunkSize(other.baseTrunkSize), allocator(std::move(other.allocator)) {
		other.baseTrunkSize = 0;
	}
	inline BoundsTreePrototype& operator=(BoundsTreePrototype&& other) noexcept {
		this->baseTrunk = std::move(other.baseTrunk);
		this->baseTrunkSize = other.baseTrunkSize;
		this->allocator = std::move(other.allocator);

		other.baseTrunkSize = 0;

		return *this;
	}

	void add(void* newObject, const BoundsTemplate<float>& bounds);
	void remove(const void* objectToRemove, const BoundsTemplate<float>& bounds);

	void addToGroup(void* newObject, const BoundsTemplate<float>& newObjectBounds, const void* groupRepresentative, const BoundsTemplate<float>& groupRepBounds);
	void mergeGroups(const void* groupRepA, const BoundsTemplate<float>& repABounds, const void* groupRepB, const BoundsTemplate<float>& repBBounds);
	void transferGroupTo(const void* groupRep, const BoundsTemplate<float>& groupRepBounds, BoundsTreePrototype& destinationTree);
	
	void updateObjectBounds(const void* object, const BoundsTemplate<float>& originalBounds, const BoundsTemplate<float>& newBounds);
	// oldObject and newObject should have the same bounds
	void findAndReplaceObject(const void* oldObject, void* newObject, const BoundsTemplate<float>& bounds);

	void disbandGroup(const void* groupRep, const BoundsTemplate<float>& groupRepBounds);

	bool contains(const void* object, const BoundsTemplate<float>& bounds) const;
	bool groupContains(const void* groupRep, const BoundsTemplate<float>& groupRepBounds, const void* object, const BoundsTemplate<float>& bounds) const;
	size_t size() const;
	size_t groupSize(const void* groupRep, const BoundsTemplate<float>& groupRepBounds) const;

	inline bool isEmpty() const {
		return this->baseTrunkSize == 0;
	}

	void clear();


	template<typename GroupIter, typename GroupIterEnd>
	void removeSubGroup(GroupIter iter, const GroupIterEnd& iterEnd) {
		if(!(iter != iterEnd)) return;
		std::pair<const void*, BoundsTemplate<float>> firstObject = *iter;
		++iter;
		if(!(iter != iterEnd)) { // just one item
			this->remove(firstObject.first, firstObject.second);
		} else {
			int newMainSize = contractGroupRecursive(this->allocator, this->baseTrunk, this->baseTrunkSize, firstObject.first, firstObject.second, [this, &iter, &iterEnd, &firstObject](TreeNodeRef& group, const BoundsTemplate<float>& groupNodeBounds) {
				if(!group.isTrunkNode()) {
					throw "Attempting to remove more than one object from a single-object group!";
				}
				TreeTrunk& groupTrunk = group.asTrunk();
				int groupTrunkSize = group.getTrunkSize();

				groupTrunkSize = removeRecursive(this->allocator, groupTrunk, groupTrunkSize, firstObject.first, firstObject.second);
				assert(groupTrunkSize != -1); // should have been verified 

				do {
					std::pair<const void*, BoundsTemplate<float>> currentlyMoving = *iter;
					groupTrunkSize = removeRecursive(this->allocator, groupTrunk, groupTrunkSize, currentlyMoving.first, currentlyMoving.second);
					if(groupTrunkSize == -1) {
						throw "Iterator provided an object which could not be found in this group!";
					}
					++iter;
				} while(iter != iterEnd);

				if(groupTrunkSize >= 2) {
					group.setTrunkSize(groupTrunkSize);
					return std::optional<BoundsTemplate<float>>(TrunkSIMDHelperFallback::getTotalBounds(groupTrunk, groupTrunkSize));
				} else if(groupTrunkSize == 1) {
					BoundsTemplate<float> resultingBounds = groupTrunk.getBoundsOfSubNode(0);
					group = std::move(groupTrunk.subNodes[0]);
					if(group.isTrunkNode()) {
						group.makeGroupHead(); // make sure to preserve the group head if the trunk is overwritten
					}
					this->allocator.freeTrunk(&groupTrunk);
					return std::optional<BoundsTemplate<float>>(resultingBounds);
				} else {
					this->allocator.freeTrunk(&groupTrunk);
					return std::optional<BoundsTemplate<float>>();
				}
			});
			this->baseTrunkSize = newMainSize;
		}
	}

	// the given iterator should return objects of type std::pair<void*, BoundsTemplate<float>>
	template<typename GroupIter, typename GroupIterEnd>
	void transferSplitGroupTo(GroupIter iter, const GroupIterEnd& iterEnd, BoundsTreePrototype& destinationTree) {
		if(!(iter != iterEnd)) return; // no items

		this->removeSubGroup(iter, iterEnd);

		std::pair<const void*, BoundsTemplate<float>> firstObject = *iter;
		++iter;
		
		if(iter != iterEnd) {
			TreeTrunk* newGroupTrunk = destinationTree.allocator.allocTrunk();
			newGroupTrunk->setSubNode(0, TreeNodeRef(const_cast<void*>(firstObject.first)), firstObject.second);
			int newGroupTrunkSize = 1;

			do {
				std::pair<const void*, BoundsTemplate<float>> obj = *iter;
				newGroupTrunkSize = addRecursive(destinationTree.allocator, *newGroupTrunk, newGroupTrunkSize, TreeNodeRef(const_cast<void*>(obj.first)), obj.second);
				++iter;
			} while(iter != iterEnd);

			destinationTree.baseTrunkSize = addRecursive(destinationTree.allocator, destinationTree.baseTrunk, destinationTree.baseTrunkSize, TreeNodeRef(newGroupTrunk, newGroupTrunkSize, true), TrunkSIMDHelperFallback::getTotalBounds(*newGroupTrunk, newGroupTrunkSize));
		} else {
			destinationTree.add(const_cast<void*>(firstObject.first), firstObject.second);
		}
	}

	// the given iterator should return objects of type std::pair<void*, BoundsTemplate<float>>
	template<typename GroupIter, typename GroupIterEnd>
	void splitGroup(GroupIter iter, const GroupIterEnd& iterEnd) {
		this->transferSplitGroupTo(iter, iterEnd, *this);
	}

	inline void improveStructure() {/*TODO*/}
	inline void maxImproveStructure() {/*TODO*/ }

	BoundsTreeIteratorPrototype begin() const { return BoundsTreeIteratorPrototype(baseTrunk, baseTrunkSize); }
	IteratorEnd end() const { return IteratorEnd(); }

	// unsafe functions
	inline std::pair<TreeTrunk&, int> getBaseTrunk() { return std::pair<TreeTrunk&, int>(this->baseTrunk, this->baseTrunkSize); }
	inline std::pair<const TreeTrunk&, int> getBaseTrunk() const { return std::pair<const TreeTrunk&, int>(this->baseTrunk, this->baseTrunkSize); }

	inline TrunkAllocator& getAllocator() { return allocator; }
	inline const TrunkAllocator& getAllocator() const { return allocator; }

	void addGroupTrunk(TreeTrunk* newNode, int newNodeSize);
};

template<typename Boundable>
void recalculateBoundsRecursive(TreeTrunk& curTrunk, int curTrunkSize) {
	for(int i = 0; i < curTrunkSize; i++) {
		TreeNodeRef& subNode = curTrunk.subNodes[i];

		if(subNode.isTrunkNode()) {
			TreeTrunk& subTrunk = subNode.asTrunk();
			int subTrunkSize = subNode.getTrunkSize();
			recalculateBoundsRecursive<Boundable>(subTrunk, subTrunkSize);
			curTrunk.setBoundsOfSubNode(i, TrunkSIMDHelperFallback::getTotalBounds(subTrunk, subTrunkSize));
		} else {
			Boundable* object = static_cast<Boundable*>(subNode.asObject());
			curTrunk.setBoundsOfSubNode(i, object->getBounds());
		}
	}
}

template<typename Boundable>
bool updateGroupBoundsRecursive(TreeTrunk& curTrunk, int curTrunkSize, const Boundable* groupRep, const BoundsTemplate<float>& originalGroupRepBounds) {
	assert(curTrunkSize >= 0 && curTrunkSize <= BRANCH_FACTOR);
	std::array<bool, BRANCH_FACTOR> couldContain = TrunkSIMDHelperFallback::getAllContainsBounds(curTrunk, originalGroupRepBounds);
	for(int i = 0; i < curTrunkSize; i++) {
		if(!couldContain[i]) continue;

		TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			TreeTrunk& subTrunk = subNode.asTrunk();
			int subTrunkSize = subNode.getTrunkSize();
			if(subNode.isGroupHead()) {
				if(containsObjectRecursive(subTrunk, subTrunkSize, groupRep, originalGroupRepBounds)) {
					recalculateBoundsRecursive<Boundable>(subTrunk, subTrunkSize);
					curTrunk.setBoundsOfSubNode(i, TrunkSIMDHelperFallback::getTotalBounds(subTrunk, subTrunkSize));
					return true;
				}
			} else {
				if(updateGroupBoundsRecursive<Boundable>(subTrunk, subTrunkSize, groupRep, originalGroupRepBounds)) {
					curTrunk.setBoundsOfSubNode(i, TrunkSIMDHelperFallback::getTotalBounds(subTrunk, subTrunkSize));
					return true;
				}
			}
		} else {
			if(subNode.asObject() == groupRep) {
				curTrunk.setBoundsOfSubNode(i, groupRep->getBounds());
				return true;
			}
		}
	}
	return false;
}

template<typename Boundable>
class BoundableCastIterator {
	BoundsTreeIteratorPrototype iter;
public:
	BoundableCastIterator() = default;
	BoundableCastIterator(BoundsTreeIteratorPrototype&& iter) : iter(std::move(iter)) {}
	BoundableCastIterator(const TreeTrunk& baseTrunk, int baseTrunkSize) : iter(baseTrunk, baseTrunkSize) {}

	BoundableCastIterator& operator++() { ++iter; return *this; }
	Boundable& operator*() const { return *static_cast<Boundable*>(*iter); }
	bool operator!=(IteratorEnd) const { return iter != IteratorEnd{}; }
};

template<typename Boundable>
class BoundsTree {
	BoundsTreePrototype tree;

public:
	inline const BoundsTreePrototype& getPrototype() const { return tree; }
	inline BoundsTreePrototype& getPrototype() { return tree; }

	template<typename BoundableIter>
	struct BoundableIteratorAdapter {
		BoundableIter iter;

		std::pair<const void*, BoundsTemplate<float>> operator*() const {
			const Boundable* item = *iter;
			return std::pair<const void*, BoundsTemplate<float>>(static_cast<const void*>(item), item->getBounds());
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

	void add(Boundable* newObject) {
		tree.add(static_cast<void*>(newObject), newObject->getBounds());
	}
	void remove(const Boundable* objectToRemove) {
		tree.remove(static_cast<const void*>(objectToRemove), objectToRemove->getBounds());
	}
	void addToGroup(Boundable* newObject, const Boundable* groupRepresentative) {
		tree.addToGroup(static_cast<void*>(newObject), newObject->getBounds(), static_cast<const void*>(groupRepresentative), groupRepresentative->getBounds());
	}
	template<typename BoundableIter, typename BoundableIterEnd>
	void addAllToGroup(BoundableIter iter, const BoundableIterEnd& iterEnd, const Boundable* groupRep) {
		if(!(iter != iterEnd)) return;
		modifyGroupRecursive(tree.allocator, tree.baseTrunk, tree.baseTrunkSize, groupRep, groupRep->getBounds(), [this, &iter, &iterEnd](TreeNodeRef& groupNode, BoundsTemplate<float> groupBounds) {
			if(groupNode.isLeafNode()) {
				TreeTrunk* newTrunk = this->tree.allocator.allocTrunk();
				newTrunk->setSubNode(0, std::move(groupNode), groupBounds);

				Boundable* newObj = *iter;
				BoundsTemplate<float> newObjBounds = newObj->getBounds();
				newTrunk->setSubNode(1, TreeNodeRef(newObj), newObjBounds);

				groupNode = TreeNodeRef(newTrunk, 2, true);
				++iter;
			}
			TreeTrunk& trunk = groupNode.asTrunk();
			int curTrunkSize = groupNode.getTrunkSize();
			while(iter != iterEnd) {
				Boundable* curObj = *iter;
				BoundsTemplate<float> curObjBounds = curObj->getBounds();

				curTrunkSize = addRecursive(this->tree.allocator, trunk, curTrunkSize, TreeNodeRef(curObj), curObjBounds);

				++iter;
			}
			return TrunkSIMDHelperFallback::getTotalBounds(trunk, curTrunkSize);
		});
	}
	void mergeGroups(const Boundable* groupRepA, const Boundable* groupRepB) {
		tree.mergeGroups(static_cast<const void*>(groupRepA), groupRepA->getBounds(), static_cast<const void*>(groupRepB), groupRepB->getBounds());
	}
	bool contains(const Boundable* object) const {
		return tree.contains(static_cast<const void*>(object), object->getBounds());
	}
	bool groupContains(const Boundable* groupRep, const Boundable* object) const {
		assert(this->contains(groupRep));
		return tree.groupContains(static_cast<const void*>(groupRep), groupRep->getBounds(), static_cast<const void*>(object), object->getBounds());
	}
	void updateObjectBounds(const Boundable* object, const BoundsTemplate<float>& originalBounds) {
		tree.updateObjectBounds(static_cast<const void*>(object), originalBounds, object->getBounds());
	}
	void updateObjectGroupBounds(const Boundable* groupRep, const BoundsTemplate<float>& originalGroupRepBounds) {
		bool success = updateGroupBoundsRecursive<Boundable>(tree.baseTrunk, tree.baseTrunkSize, groupRep, originalGroupRepBounds);
		if(!success) throw "groupRep was not found in tree!";
	}
	// oldObject and newObject should have the same bounds
	void findAndReplaceObject(const Boundable* oldObject, Boundable* newObject, const BoundsTemplate<float>& bounds) {
		assert(this->tree.contains(oldObject, bounds));
		tree.findAndReplaceObject(static_cast<const void*>(oldObject), static_cast<void*>(newObject), bounds);
	}
	void disbandGroup(const Boundable* groupRep) {
		assert(this->contains(groupRep));
		tree.disbandGroup(static_cast<const void*>(groupRep), groupRep->getBounds());
	}
	size_t size() const {
		return tree.size();
	}
	size_t groupSize(const Boundable* groupRep) const {
		return tree.groupSize(static_cast<const void*>(groupRep), groupRep->getBounds());
	}
	bool isEmpty() const {
		return tree.isEmpty();
	}
	void clear() {
		tree.clear();
	}

	void transferGroupTo(const Boundable* groupRep, BoundsTree& destinationTree) {
		tree.transferGroupTo(static_cast<const void*>(groupRep), groupRep->getBounds(), destinationTree.tree);
	}

	// the given iterator should return objects of type Boundable*
	template<typename GroupIter, typename GroupIterEnd>
	void transferSplitGroupTo(GroupIter&& iter, const GroupIterEnd& iterEnd, BoundsTree& destinationTree) {
		tree.transferSplitGroupTo(BoundableIteratorAdapter<GroupIter>{std::move(iter)}, iterEnd, destinationTree.tree);
	}

	void transferTo(Boundable* obj, BoundsTree& destinationTree) {
		BoundsTemplate<float> bounds = obj->getBounds();
		tree.remove(static_cast<void*>(obj), bounds);
		destinationTree.tree.add(static_cast<void*>(obj), bounds);
	}

	void moveOutOfGroup(Boundable* obj) {
		BoundsTemplate<float> bounds = obj->getBounds();
		tree.remove(static_cast<void*>(obj), bounds);
		tree.add(static_cast<void*>(obj), bounds);
	}

	// the given iterator should return objects of type Boundable*
	template<typename GroupIter, typename GroupIterEnd>
	void splitGroup(GroupIter iter, const GroupIterEnd& iterEnd) {
		tree.splitGroup(BoundableIteratorAdapter<GroupIter>{std::move(iter)}, iterEnd);
	}

	// expects a function of the form void(Boundable& object)
	template<typename Func>
	void forEach(const Func& func) const {
		forEachRecurse<Boundable, Func>(this->tree.baseTrunk, this->tree.baseTrunkSize, func);
	}

	// expects a function of the form void(Boundable& object)
	template<typename Func>
	void forEachInGroup(const void* groupRepresentative, const BoundsTemplate<float>& groupRepBounds, const Func& func) const {
		const TreeNodeRef* group = getGroupRecursive(this->tree.baseTrunk, this->tree.baseTrunkSize, groupRepresentative, groupRepBounds);
		if(group == nullptr) {
			throw "Group not found!";
		}
		if(group->isTrunkNode()) {
			forEachRecurse<Boundable, Func>(group->asTrunk(), group->getTrunkSize(), func);
		} else {
			func(*static_cast<Boundable*>(group->asObject()));
		}
	}

	BoundableCastIterator<Boundable> begin() const { return BoundableCastIterator<Boundable>(this->tree.begin()); }
	IteratorEnd end() const { return IteratorEnd(); }

	template<typename Func>
	void forEachColission(const Func& func) const {
		forEachColissionInternalRecursive<Boundable, TrunkSIMDHelperFallback, Func>(this->tree.baseTrunk, this->tree.baseTrunkSize, func);
	}

	template<typename Func>
	void forEachColissionWith(const BoundsTree& other, const Func& func) const {
		forEachColissionBetweenRecursive<Boundable, TrunkSIMDHelperFallback, Func>(this->tree.baseTrunk, this->tree.baseTrunkSize, other.tree.baseTrunk, other.tree.baseTrunkSize, func);
	}

	void recalculateBounds() {
		recalculateBoundsRecursive<Boundable>(this->tree.baseTrunk, this->tree.baseTrunkSize);
	}

	void improveStructure() {/*TODO*/ }
	void maxImproveStructure() {/*TODO*/ }
};

struct BasicBounded {
	BoundsTemplate<float> bounds;
	BoundsTemplate<float> getBounds() const { return bounds; }
};

};

