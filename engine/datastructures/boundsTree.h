#pragma once

#include "buffers.h"
#include "iteratorFactory.h"
#include "iteratorEnd.h"

#include "../math/position.h"
#include "../math/fix.h"
#include "../math/bounds.h"
#include "../math/ray.h"

#include <utility>
#include <new>


#define MAX_BRANCHES 4
#define MAX_HEIGHT 64
#define LEAF_NODE_SIGNIFIER 0x7FFFFFFF

struct TreeNode {
	Bounds bounds;
	union {
		TreeNode* subTrees;
		void* object;
	};
	int nodeCount;
	/* means that the nodes within this node belong to a specific group, if true, the tree will not separate the elements below this one. 
	New elements will not be added to this group unless specifically specified
	If false, then no subnodes are allowed to be exchanged with the rest of the tree. This node must be viewed as a black box. 
	*/
	bool isGroupHead = false;

	inline bool isLeafNode() const { return nodeCount == LEAF_NODE_SIGNIFIER; }

	inline TreeNode() : nodeCount(LEAF_NODE_SIGNIFIER), object(nullptr) {}
	inline TreeNode(TreeNode* subTrees, int nodeCount);
	inline TreeNode(void* object, const Bounds& bounds) : nodeCount(LEAF_NODE_SIGNIFIER), object(object), bounds(bounds) {}
	inline TreeNode(void* object, const Bounds& bounds, bool isGroupHead) : nodeCount(LEAF_NODE_SIGNIFIER), object(object), bounds(bounds), isGroupHead(isGroupHead) {}
	inline TreeNode(const Bounds& bounds, TreeNode* subTrees, int nodeCount) : bounds(bounds), subTrees(subTrees), nodeCount(nodeCount) {}

	inline TreeNode(const TreeNode&) = delete;
	inline void operator=(const TreeNode&) = delete;

	inline TreeNode(TreeNode&& other) noexcept : nodeCount(other.nodeCount), subTrees(other.subTrees), bounds(other.bounds), isGroupHead(other.isGroupHead) {
		other.subTrees = nullptr;
		other.nodeCount = LEAF_NODE_SIGNIFIER;
	}
	inline TreeNode& operator=(TreeNode&& other) noexcept {
		std::swap(this->nodeCount, other.nodeCount);
		std::swap(this->subTrees, other.subTrees);
		std::swap(this->bounds, other.bounds);
		std::swap(this->isGroupHead, other.isGroupHead);
		return *this;
	}
	
	inline TreeNode* begin() const { return subTrees; }
	inline TreeNode* end() const { return subTrees+nodeCount; }
	inline TreeNode& operator[](int index) { return subTrees[index]; }
	inline const TreeNode& operator[](int index) const { return subTrees[index]; }

	inline ~TreeNode();


	void addOutside(TreeNode&& newNode);
	void addInside(TreeNode&& newNode);
	inline void remove(int index) {
		--nodeCount;
		if (index != nodeCount)
			subTrees[index] = std::move(subTrees[nodeCount]);

		subTrees[nodeCount].~TreeNode();

		if (nodeCount == 1) {
			TreeNode* buf = subTrees;
			bool resultDivisible = this->isGroupHead || buf[0].isGroupHead;
			new(this) TreeNode(std::move(buf[0]));
			this->isGroupHead = resultDivisible;
			delete[] buf;
		}
	}

	void recalculateBounds();
	void recalculateBoundsFromSubBounds();
	void recalculateBoundsRecursive();

	void improveStructure();
};

long long computeCost(const Bounds& bounds);

Bounds computeBoundsOfList(const TreeNode* const* list, size_t count);

Bounds computeBoundsOfList(const TreeNode* list, size_t count);

struct TreeStackElement {
	TreeNode* node;
	int index;
};

extern TreeNode dummy_TreeNode;

struct NodeStack {
	TreeStackElement* top;
	TreeStackElement stack[MAX_HEIGHT];

	NodeStack(TreeNode& rootNode) : stack{ TreeStackElement{&rootNode, 0} }, top(stack) {
		if (rootNode.nodeCount == 0) {
			top--;
		}
	}
	// a find function, returning the stack of all nodes leading up to the requested object
	NodeStack(TreeNode& rootNode, const void* objToFind, const Bounds& objBounds) : NodeStack(rootNode) {
		if (rootNode.isLeafNode()) {
			if (rootNode.object == objToFind) {
				return;
			} else {
				throw "Could not find obj in Tree!";
			}
		}
		while (top + 1 != stack) {
			if (top->index != top->node->nodeCount) {
				TreeNode* nextNode = top->node->subTrees + top->index;
				if (nextNode->bounds.contains(objBounds)) {
					if (nextNode->isLeafNode()) {
						if (nextNode->object == objToFind) {
							top++;
							*top = TreeStackElement{ nextNode, 0 };
							return;
						} else {
							top->index++;
						}
					} else {
						top++;
						*top = TreeStackElement{ nextNode, 0 };
					}
				} else {
					top->index++;
				}
			} else {
				top--;
				top->index++;
			}
		}

		throw "Could not find obj in Tree!";
	}
	NodeStack(const NodeStack& other) : stack{}, top(this->stack + (other.top - other.stack)) {
		for (int i = 0; i < top - stack + 1; i++) {
			this->stack[i] = other.stack[i];
		}
	}
	NodeStack(NodeStack&& other) noexcept : stack{}, top(this->stack + (other.top - other.stack)) {
		for (int i = 0; i < top - stack + 1; i++) {
			this->stack[i] = other.stack[i];
		}
	}
	NodeStack& operator=(const NodeStack& other) {
		this->top = this->stack + (other.top - other.stack);
		for (int i = 0; i < top - stack + 1; i++) {
			this->stack[i] = other.stack[i];
		}
		return *this;
	}
	NodeStack& operator=(NodeStack&& other) noexcept {
		this->top = this->stack + (other.top - other.stack);
		for (int i = 0; i < top - stack + 1; i++) {
			this->stack[i] = other.stack[i];
		}
		return *this;
	}
	inline bool operator!=(IteratorEnd) const {
		return top + 1 != stack;
	}
	inline TreeNode* operator*() const {
		return top->node;
	}
	inline void riseUntilAvailableWhile() {
		while (top->index == top->node->nodeCount) {
			top--;
			if (top < stack) return;
			top->index++;
		}
	}
	inline void riseUntilAvailableDoWhile() {
		do {
			top--;
			if (top < stack) return;
			top->index++;
		} while (top->index == top->node->nodeCount);
	}

	inline void riseUntilGroupHeadDoWhile() {
		do {
			top--;
			if (top < stack) throw "Did not find groupHead node above!";
		} while (!top->node->isGroupHead);
	}

	inline void riseUntilGroupHeadWhile() {
		while (!top->node->isGroupHead) {
			top--;
			if (top < stack) throw "Did not find groupHead node above!";
		};
	}

	inline void updateBoundsAllTheWayToTop() {
		TreeStackElement* newTop = top - 1;
		while (newTop + 1 != stack) {
			TreeNode* n = newTop->node;
			n->recalculateBoundsFromSubBounds();
			newTop--;
		}
	}

	// removes the object currently pointed to
	inline void remove() {
		do {
			top--;
			top->node->remove(top->index);
		} while (top->node->nodeCount == 0);
		if (top->node->isLeafNode()) {
			top--;
			top->index++;
		}
		updateBoundsAllTheWayToTop();
		riseUntilAvailableWhile();
	}
};

struct ConstTreeIterator : public NodeStack {
	ConstTreeIterator(TreeNode& rootNode) : NodeStack(rootNode) {
		// the very first element is a dummy, in order to detect when the tree is done
		
		if (rootNode.nodeCount == 0) return;
		
		while(!top->node->isLeafNode())	{
			TreeNode* nextNode = &top->node->subTrees[top->index];
			top++;
			top->node = nextNode;
			top->index = 0;
		} 
	}
	inline void delveDown() {
		do {
			TreeNode* nextNode = &top->node->subTrees[top->index];
			top++;
			top->node = nextNode;
			top->index = 0;
		} while (!top->node->isLeafNode());
	}
	inline void operator++() {
		// go back up until a new available node is found
		do {
			top--;
			if (top < stack) return;
			top->index++;
		} while (top->index == top->node->nodeCount);
		// delve down until a feasible leaf node is found
		delveDown();
	}
};

struct TreeIterator : public ConstTreeIterator {
	TreeIterator(TreeNode& rootNode) : ConstTreeIterator(rootNode) {
		// the very first element is a dummy, in order to detect when the tree is done

		if (rootNode.nodeCount == 0) return;
	}
	inline void remove() {
		NodeStack::remove();
		if (top < stack) return;
		delveDown();
	}
};

/*
	Iterates through the tree, applying Filter at every level to cull branches that should not be searched

	Filter must define an operator(Bounds) returning true if the filter passes for this bound, and false if it does not.

	For correct operation, it must abide by the following:
	- If the filter returns true for some bound, then it must also return true for any bound fully encompassing the first bound. 
	- If the filter returns false for some bound, then it must return false for all bounds it encompasses. 

	Correct filter:
	- BoundIntersectsRay
	    If the bound intersects a ray, then this ray must also intersect it's parent
		If the bound does not intersect the ray, then it's children cannot
	
	Incorrect filter:
	- BoundsContainedIn
		If a given bound is contained in another bound2, that does not mean that it's parent must also be contained in this bound2
		However, BoundsNotContainedIn IS a correct filter
*/
template<typename Filter>
struct FilteredTreeIterator : public NodeStack {
	Filter filter;
	FilteredTreeIterator(TreeNode& rootNode, const Filter& filter) : NodeStack(rootNode), filter(filter) {
		// the very first element is a dummy, in order to detect when the tree is done
		if (rootNode.nodeCount == 0) return;

		if(!rootNode.isLeafNode())
			delveDownFiltered();
	}

	void delveDownFiltered() {
		while (true) {
			// go down
			TreeNode* nextNode = &top->node->subTrees[top->index];
			top++;
			top->node = nextNode;

			if (filter(*nextNode)) {
				if (nextNode->isLeafNode()) {
					return;
				} else {
					top->index = 0;
				}
			} else {
				top--;
				top->index++;
				while (top->index == top->node->nodeCount) {
					top--;
					if (top < stack) return;
					top->index++;
				}
			}
		}
	}

	inline void operator++() {
		// go back up until a new available node is found
		do {
			top--;
			if (top < stack) return;
			top->index++;
		} while (top->index == top->node->nodeCount);

		delveDownFiltered();
	}
	inline void remove() {
		NodeStack::remove();
		if (top < stack) return;
		delveDownFiltered();
	}
};
/*
inline bool containsFilterBounds(const Bounds& nodeBounds, const Bounds& filterBounds) { return nodeBounds.contains(filterBounds); }
inline bool containsFilterPoint(const Bounds& nodeBounds, const Position& filterPos) { return nodeBounds.contains(filterPos); }

typedef FilteredTreeIterator<Bounds, intersects> TreeIteratorIntersectingBounds;
typedef FilteredTreeIterator<Ray, doRayAndBoundsIntersect> TreeIteratorIntersectingRay;
typedef FilteredTreeIterator<Bounds, containsFilterBounds> TreeIteratorContainingBounds;
typedef FilteredTreeIterator<Position, containsFilterPoint> TreeIteratorContainingPoint;
*/



template<typename Iter, typename Boundable>
struct BoundsTreeIter {
	Iter iter;
	BoundsTreeIter(const Iter& iter) : iter(iter) {}
	void operator++() {
		++iter;
	}
	Boundable& operator*() const {
		return *static_cast<Boundable*>((*iter)->object);
	}
	bool operator!=(IteratorEnd) const {
		return iter != IteratorEnd();
	}
};

template<typename Filter, typename Boundable>
using FilteredBoundsTreeIter = BoundsTreeIter<FilteredTreeIterator<Filter>, Boundable>;

template<typename Iter>
struct TreeIterFactory : IteratorFactory<Iter, IteratorEnd> {
	TreeIterFactory(const Iter& iter) : IteratorFactory<Iter, IteratorEnd>(iter, IteratorEnd()) {}
	TreeIterFactory(Iter&& iter) : IteratorFactory<Iter, IteratorEnd>(std::move(iter), IteratorEnd()) {}
};

struct FinderFilter {
	Bounds filterBounds;

	FinderFilter() = default;
	FinderFilter(const Bounds& filterBounds) : filterBounds(filterBounds) {}

	bool operator()(const TreeNode& b) const { return b.bounds.contains(filterBounds); }
};

template<typename Boundable>
struct BoundsTree {
	TreeNode rootNode;

	BoundsTree() : rootNode(new TreeNode[MAX_BRANCHES], 0) {

	}

	void add(Boundable* obj, const Bounds& bounds) {
		this->rootNode.addOutside(TreeNode(obj, bounds, true));
	}

	void addToExistingGroup(Boundable* obj, const Bounds& bounds, TreeNode& groupNode) {
		groupNode.addInside(TreeNode(obj, bounds, false));
	}

	void addToExistingGroup(Boundable* obj, const Bounds& bounds, const Boundable* objInGroup, const Bounds& objInGroupBounds) {
		NodeStack iter(rootNode, objInGroup, objInGroupBounds);
		iter.riseUntilGroupHeadWhile();
		addToExistingGroup(obj, bounds, **iter);
		iter.updateBoundsAllTheWayToTop();
		TreeStackElement* newTop = iter.top;
		while (newTop + 1 != iter.stack) {
			TreeNode* n = newTop->node;
			n->bounds = unionOfBounds(n->bounds, bounds);
			newTop--;
		}
	}

	void remove(const Boundable* obj, const Bounds& strictBounds) {
		NodeStack stack(rootNode, obj, strictBounds);
		stack.remove();
	}
	void remove(const Boundable* obj) {
		this->remove(obj, obj->getStrictBounds());
	}

	inline void recalculateBounds() {
		for (TreeNode* currentNode : *this) {
			Boundable* obj = static_cast<Boundable*>(currentNode->object);
			currentNode->bounds = obj->getStrictBounds();
		}

		rootNode.recalculateBoundsRecursive();
	}
	
	void updateObjectBounds(const Boundable* obj, const Bounds& oldBounds) {
		NodeStack stack(rootNode, obj, oldBounds);
		stack.top->node->bounds = obj->getStrictBounds();

		stack.updateBoundsAllTheWayToTop();
	}
	void updateObjectGroupBounds(const Boundable* objInGroup, const Bounds& objOldBounds) {
		NodeStack stack(rootNode, objInGroup, objOldBounds);
		stack.riseUntilGroupHeadWhile(); // find group obj belongs to

		for (TreeIterator iter(*stack.top->node); iter != IteratorEnd(); ++iter) {
			TreeNode* node = *iter;
			node->bounds = static_cast<Boundable*>(node->object)->getStrictBounds();
		}
		stack.top->node->recalculateBoundsRecursive(); // refresh group bounds

		stack.updateBoundsAllTheWayToTop(); // refresh rest of tree to accommodate
	}

	inline void improveStructure() { rootNode.improveStructure(); }
	
	inline TreeIterator begin() { return TreeIterator(rootNode); };
	inline ConstTreeIterator begin() const { return ConstTreeIterator(const_cast<TreeNode&>(rootNode)); };
	inline IteratorEnd end() const { return IteratorEnd(); };

	template<typename Filter>
	inline TreeIterFactory<FilteredBoundsTreeIter<Filter, Boundable>> iterFiltered(const Filter& filter) {
		return TreeIterFactory<FilteredBoundsTreeIter<Filter, Boundable>>(FilteredBoundsTreeIter<Filter, Boundable>(FilteredTreeIterator<Filter>(this->rootNode, filter)));
	}
};
