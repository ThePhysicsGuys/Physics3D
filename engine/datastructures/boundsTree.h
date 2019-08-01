#pragma once

#include "buffers.h"

#include "../math/position.h"
#include "../math/fix.h"
#include "../math/bounds.h"

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
struct TreeIteratorEnd {};

struct TreeIterBase {
	TreeStackElement stack[MAX_HEIGHT];
	TreeStackElement* top;

	TreeIterBase(TreeNode& rootNode) : stack{ TreeStackElement{&rootNode, 0} }, top(stack) {
		if (rootNode.nodeCount == 0) {
			top--;
		}
	}
	inline bool operator!=(TreeIteratorEnd) const {
		return top + 1 != stack;
	}
	inline TreeNode* operator*() const {
		return top->node;
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
	inline void operator++() {
		// go back up until a new available node is found
		do {
			top--;
			if (top < stack) return;
			top->index++;
		} while (top->index == top->node->nodeCount);
		// delve down until a feasible leaf node is found
		do {
			TreeNode* nextNode = &top->node->subTrees[top->index];
			top++;
			top->node = nextNode;
			top->index = 0;
		} while (!top->node->isLeafNode());
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
};

inline bool containsFilterBounds(const Bounds& nodeBounds, const Bounds& filterBounds) { return nodeBounds.contains(filterBounds); }
inline bool containsFilterPoint(const Bounds& nodeBounds, const Position& filterPos) { return nodeBounds.contains(filterPos); }

typedef FilteredTreeIterator<Bounds, intersects> TreeIteratorIntersectingBounds;
typedef FilteredTreeIterator<Bounds, containsFilterBounds> TreeIteratorContainingBounds;
typedef FilteredTreeIterator<Position, containsFilterPoint> TreeIteratorContainingPoint;

template<typename Boundable>
struct BoundsTree {
	TreeNode rootNode;

	BoundsTree() : rootNode(new TreeNode[MAX_BRANCHES], 0) {

	}

	void add(Boundable* obj, bool strictBounds) {
		rootNode.add(TreeNode(obj, strictBounds?obj->getStrictBounds():obj->getLooseBounds()));
	}

	void remove(Boundable* obj, const Bounds& strictBounds) {

	}

	inline void recalculateBounds(bool strictBounds) {
		for (TreeNode* currentNode : *this) {
			Boundable* obj = static_cast<Boundable*>(currentNode->object);
			currentNode->bounds = strictBounds ? obj->getStrictBounds() : obj->getLooseBounds();
		}

		rootNode.recalculateBoundsRecursive();
	}
	inline void improveStructure() { rootNode.improveStructure(); }
	
	//unsigned long long computeTreeScore() const;

	inline TreeIterator begin() { return TreeIterator(rootNode); };
	inline TreeIteratorEnd end() { return TreeIteratorEnd(); };
};
