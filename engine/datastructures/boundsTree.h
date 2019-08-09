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
private:
	void addToSubTrees(TreeNode&& newNode);
public:
	Bounds bounds;
	union {
		TreeNode* subTrees;
		void* object;
	};
	int nodeCount;

	inline bool isLeafNode() const { return nodeCount == LEAF_NODE_SIGNIFIER; }

	inline TreeNode() : nodeCount(LEAF_NODE_SIGNIFIER), object(nullptr) {};
	inline TreeNode(TreeNode* subTrees, int nodeCount) : subTrees(subTrees), nodeCount(nodeCount) {}
	inline TreeNode(void* object, const Bounds& bounds) : nodeCount(LEAF_NODE_SIGNIFIER), object(object), bounds(bounds) {}
	inline TreeNode(const Bounds& bounds, TreeNode* subTrees, int nodeCount) : bounds(bounds), subTrees(subTrees), nodeCount(nodeCount) {}

	inline TreeNode(const TreeNode&) = delete;
	inline void operator=(const TreeNode&) = delete;

	inline TreeNode(TreeNode&& other) noexcept : nodeCount(other.nodeCount), subTrees(other.subTrees), bounds(other.bounds) {
		other.subTrees = nullptr;
		other.nodeCount = LEAF_NODE_SIGNIFIER;
	}
	inline TreeNode& operator=(TreeNode&& other) noexcept {
		int nc = this->nodeCount; this->nodeCount = other.nodeCount; other.nodeCount = nc;
		TreeNode* n = this->subTrees; this->subTrees = other.subTrees; other.subTrees = n;
		Bounds b = this->bounds; this->bounds = other.bounds; other.bounds = b;
		return *this;
	}

	inline TreeNode* begin() const { return subTrees; }
	inline TreeNode* end() const { return subTrees+nodeCount; }
	inline TreeNode& operator[](int index) { return subTrees[index]; }
	inline const TreeNode& operator[](int index) const { return subTrees[index]; }

	inline ~TreeNode();

	void add(TreeNode&& obj);
	inline void remove(int index) {
		--nodeCount;
		if (index != nodeCount)
			subTrees[index] = std::move(subTrees[nodeCount]);

		subTrees[nodeCount].~TreeNode();

		if (nodeCount == 1) {
			TreeNode* buf = subTrees;
			new(this) TreeNode(std::move(buf[0]));
			delete[] buf;
		}
	}

	void recalculateBounds();
	void recalculateBoundsFromSubBounds();
	void recalculateBoundsRecursive();

	void improveStructure();
};

long long computeCost(const Bounds& bounds);

struct TreeStackElement {
	TreeNode* node;
	int index;
};

extern TreeNode dummy_TreeNode;

struct TreeIterBase {
	TreeStackElement stack[MAX_HEIGHT];
	TreeStackElement* top;

	TreeIterBase(TreeNode& rootNode) : stack{ TreeStackElement{&rootNode, 0} }, top(stack) {
		if (rootNode.nodeCount == 0) {
			top--;
		}
	}
	TreeIterBase(const TreeIterBase& other) : stack{}, top(this->stack + (other.top - other.stack)) {
		for (int i = 0; i < top - stack + 1; i++) {
			this->stack[i] = other.stack[i];
		}
	}
	TreeIterBase(TreeIterBase&& other) noexcept : stack{}, top(this->stack + (other.top - other.stack)) {
		for (int i = 0; i < top - stack + 1; i++) {
			this->stack[i] = other.stack[i];
		}
	}
	TreeIterBase& operator=(const TreeIterBase& other) {
		this->top = this->stack + (other.top - other.stack);
		for (int i = 0; i < top - stack + 1; i++) {
			this->stack[i] = other.stack[i];
		}
		return *this;
	}
	TreeIterBase& operator=(TreeIterBase&& other) noexcept {
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
		riseUntilAvailableWhile();
	}
};

struct TreeIterator : public TreeIterBase {
	TreeIterator(TreeNode& rootNode) : TreeIterBase(rootNode) {
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
	inline void remove() {
		TreeIterBase::remove();
		if (top < stack) return;
		delveDown();
	}
};

template<typename Filter, bool (*filterFunc)(const Bounds& nodeBounds, const Filter& filter)>
struct FilteredTreeIterator : public TreeIterBase {
	Filter filter;
	FilteredTreeIterator(TreeNode& rootNode, const Filter& filter) : TreeIterBase(rootNode), filter(filter) {
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

			if (filterFunc(nextNode->bounds, filter)) {
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
		TreeIterBase::remove();
		if (top < stack) return;
		delveDownFiltered();
	}
};

inline bool containsFilterBounds(const Bounds& nodeBounds, const Bounds& filterBounds) { return nodeBounds.contains(filterBounds); }
inline bool containsFilterPoint(const Bounds& nodeBounds, const Position& filterPos) { return nodeBounds.contains(filterPos); }

typedef FilteredTreeIterator<Bounds, intersects> TreeIteratorIntersectingBounds;
typedef FilteredTreeIterator<Ray, doRayAndBoundsIntersect> TreeIteratorIntersectingRay;
typedef FilteredTreeIterator<Bounds, containsFilterBounds> TreeIteratorContainingBounds;
typedef FilteredTreeIterator<Position, containsFilterPoint> TreeIteratorContainingPoint;




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

template<typename Iter>
struct TreeIterFactory : IteratorFactory<Iter, IteratorEnd> {
	TreeIterFactory(const Iter& iter) : IteratorFactory<Iter, IteratorEnd>(iter, IteratorEnd()) {}
	TreeIterFactory(Iter&& iter) : IteratorFactory<Iter, IteratorEnd>(std::move(iter), IteratorEnd()) {}
};

template<typename Boundable>
struct BoundsTree {
	TreeNode rootNode;

	BoundsTree() : rootNode(new TreeNode[MAX_BRANCHES], 0) {

	}

	void add(Boundable* obj, bool strictBounds) {
		rootNode.add(TreeNode(obj, strictBounds?obj->getStrictBounds():obj->getLooseBounds()));
	}

	void remove(Boundable* obj, const Bounds& strictBounds) {
		for (TreeIteratorContainingBounds iter(rootNode, strictBounds); iter != IteratorEnd(); iter) {
			if ((*iter)->object == obj) {
				iter.remove();
				return;
			}
		}
		throw "Object to remove not found!";
	}
	void remove(Boundable* obj) {
		this->remove(obj, obj->getStrictBounds());
	}

	inline void recalculateBounds(bool strictBounds) {
		for (TreeNode* currentNode : *this) {
			Boundable* obj = static_cast<Boundable*>(currentNode->object);
			currentNode->bounds = strictBounds ? obj->getStrictBounds() : obj->getLooseBounds();
		}

		rootNode.recalculateBoundsRecursive();
	}
	inline void improveStructure() { rootNode.improveStructure(); }
	
	inline TreeIterator begin() { return TreeIterator(rootNode); };
	inline IteratorEnd end() { return IteratorEnd(); };

	inline TreeIterFactory<BoundsTreeIter<TreeIteratorIntersectingBounds, Boundable>> iterObjectsIntersectingBounds(const Bounds& bounds) {
		return TreeIterFactory<BoundsTreeIter<TreeIteratorIntersectingBounds, Boundable>>(BoundsTreeIter<TreeIteratorIntersectingBounds, Boundable>(TreeIteratorIntersectingBounds(this->rootNode, bounds)));
	}
	inline TreeIterFactory<BoundsTreeIter<TreeIteratorIntersectingRay, Boundable>> iterObjectsIntersectingRay(const Ray& ray) {
		return TreeIterFactory<BoundsTreeIter<TreeIteratorIntersectingRay, Boundable>>(BoundsTreeIter<TreeIteratorIntersectingRay, Boundable>(TreeIteratorIntersectingRay(this->rootNode, ray)));
	}
	inline TreeIterFactory<BoundsTreeIter<TreeIteratorContainingBounds, Boundable>> iterObjectsContainingBounds(const Bounds& bounds) {
		return TreeIterFactory<BoundsTreeIter<TreeIteratorContainingBounds, Boundable>>(BoundsTreeIter<TreeIteratorContainingBounds, Boundable>(TreeIteratorContainingBounds(this->rootNode, bounds)));
	}
	inline TreeIterFactory<BoundsTreeIter<TreeIteratorContainingPoint, Boundable>> iterObjectsContainingPoint(const Position& point) {
		return TreeIterFactory<BoundsTreeIter<TreeIteratorContainingPoint, Boundable>>(BoundsTreeIter<TreeIteratorContainingPoint, Boundable>(TreeIteratorContainingPoint(this->rootNode, point)));
	}
};
