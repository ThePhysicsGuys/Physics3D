#pragma once

#include "../physical.h"
#include "buffers.h"

#include "../math/position.h"
#include "../math/fix.h"
#include "../math/bounds.h"

typedef Physical Boundable;

#define MAX_BRANCHES 4


struct BoundedPhysical {
	Bounds bounds;
	Physical* object;
};
struct BoundedPhysicalGroup {
	BoundedPhysical physicals[MAX_BRANCHES];
	int physicalCount;

	const BoundedPhysical* begin() const { return &physicals[0]; }
	BoundedPhysical* begin() { return &physicals[0]; }

	const BoundedPhysical* end() const { return &physicals[physicalCount]; }
	BoundedPhysical* end() { return &physicals[physicalCount]; }
};

struct NodeGroup;

struct TreeNode {
private:
	void addToSubTrees(Boundable* obj, const Bounds& bounds);
public:
	Bounds bounds;
	union {
		NodeGroup* subTrees;
		BoundedPhysicalGroup* physicals;
	};
	bool isLeafNode;

	inline TreeNode() : isLeafNode(true), physicals(nullptr) {};
	inline explicit TreeNode(BoundedPhysicalGroup* physicals) : isLeafNode(true), physicals(physicals) {
		if (physicals->physicalCount != 0) {
			this->bounds = physicals->physicals[0].bounds;
			for (int i = 1; i < physicals->physicalCount; i++) {
				this->bounds = unionOfBounds(this->bounds, physicals->physicals[i].bounds);
			}
		}
	};
	inline TreeNode(BoundedPhysicalGroup* first, BoundedPhysicalGroup* second);

	inline TreeNode(const TreeNode&) = delete;
	inline void operator=(const TreeNode&) = delete;

	inline TreeNode(TreeNode&& other) : isLeafNode(other.isLeafNode), subTrees(other.subTrees), bounds(other.bounds) {
		other.subTrees = nullptr;
	}

	inline TreeNode& operator=(TreeNode&& other) {
		std::swap(this->isLeafNode, other.isLeafNode);
		std::swap(this->subTrees, other.subTrees);
		std::swap(this->bounds, other.bounds);
		return *this;
	}

	inline ~TreeNode();

	void splitForNewObj(Boundable* newObj, const Bounds& newObjBounds);

	void add(Boundable* obj, const Bounds& bounds);

	void recalculateBounds();
};

struct NodeGroup {
	TreeNode subNodes[MAX_BRANCHES];
	int nodeCount;

	const TreeNode* begin() const { return &subNodes[0]; }
	TreeNode* begin() { return &subNodes[0]; }

	const TreeNode* end() const { return &subNodes[nodeCount]; }
	TreeNode* end() { return &subNodes[nodeCount]; }
};

struct BoundsTree {
	TreeNode rootNode;

	BoundsTree() : rootNode(new BoundedPhysicalGroup()) {

	}

	TreeNode* findLeafNodeFor(const Bounds& bounds) const {
		const TreeNode* currentNode = &rootNode;
		while (!currentNode->isLeafNode) {

		}
	}

	void add(Boundable* obj) {
		rootNode.add(obj, obj->getStrictBounds());
	}
};
