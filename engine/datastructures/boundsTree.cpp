#include "boundsTree.h"

inline Fix<32> sumPositives(const Vec3Fix& v) {
	return (v.x.value > 0 ? v.x : Fix<32>(0LL)) + (v.y.value > 0 ? v.y : Fix<32>(0LL)) + (v.z.value > 0 ? v.z : Fix<32>(0LL));
}

/*
Computes a metric to show the cost of combining two boundingboxes
*/
inline Fix<32> computeCombinationCost(const Bounds & newBox, const Bounds & expandingBox) {
	Bounds combinedBounds = unionOfBounds(newBox, expandingBox);
	Vec3Fix diag = combinedBounds.getDiagonal();
	Fix<32> sumOfDiag = diag.x + diag.y + diag.z;
	Fix<32> productOfDiag = quickMultiply(quickMultiply(diag.x, diag.y), diag.z);
	return sumOfDiag;
}

TreeNode::~TreeNode() {
	if (!isLeafNode()) {
		delete[] subTrees;
	}
}

void TreeNode::addToSubTrees(Boundable * obj, const Bounds & bounds) {
	Fix<32> bestCost = computeCombinationCost(bounds, subTrees[0].bounds);
	int bestIndex = 0;
	for (int i = 1; i < this->nodeCount; i++) {
		Fix<32> newCost = computeCombinationCost(bounds, subTrees[i].bounds);
		if (newCost < bestCost) {
			bestCost = newCost;
			bestIndex = i;
		}
	}
	Log::debug("bestIndex: %d", bestIndex);
	subTrees[bestIndex].add(obj, bounds);
}

void TreeNode::add(Boundable * obj, const Bounds & bounds) {
	if (isLeafNode()) {
		TreeNode* newNodes = new TreeNode[MAX_BRANCHES];

		newNodes[0].object = this->object;
		newNodes[0].bounds = this->bounds;

		newNodes[1].object = obj;
		newNodes[1].bounds = bounds;

		this->nodeCount = 2;
		this->subTrees = newNodes;
	} else {
		if (nodeCount != MAX_BRANCHES) {
			new(&subTrees[nodeCount++]) TreeNode(obj, bounds);
		} else {
			addToSubTrees(obj, bounds);
		}
	}
	this->bounds = unionOfBounds(this->bounds, bounds);
}

void TreeNode::recalculateBounds(bool strictBounds) {
	if (isLeafNode()) {
		this->bounds = strictBounds ? object->getStrictBounds() : object->getLooseBounds();
	} else {
		this->bounds = subTrees[0].bounds;
		for (int i = 1; i < nodeCount; i++) {
			this->bounds = unionOfBounds(this->bounds, subTrees[i].bounds);
		}
	}
}

void TreeNode::recalculateBoundsRecursive(bool strictBounds) {
	if (!isLeafNode()) {
		for (size_t i = 0; i < nodeCount; i++) {
			subTrees[i].recalculateBoundsRecursive(strictBounds);
		}
	}

	recalculateBounds(strictBounds);
}

void TreeNode::improveStructure() {

}

void BoundsTree::update(bool strictBounds) {
	rootNode.recalculateBoundsRecursive(strictBounds);

	rootNode.improveStructure();
}