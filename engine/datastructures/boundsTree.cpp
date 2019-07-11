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

TreeNode::TreeNode(BoundedPhysicalGroup * first, BoundedPhysicalGroup * second) : isLeafNode(false), subTrees(new NodeGroup()) {
	subTrees->subNodes[0] = TreeNode(first);
	subTrees->subNodes[1] = TreeNode(second);

	this->bounds = unionOfBounds(subTrees->subNodes[0].bounds, subTrees->subNodes[1].bounds);
}
TreeNode::~TreeNode() {
	if (isLeafNode) {
		delete physicals;
	} else {
		delete subTrees;
	}
}

void TreeNode::splitForNewObj(Boundable * newObj, const Bounds & newObjBounds) {
	/*BoundedPhysicalGroup* newGroup = new BoundedPhysicalGroup();
	newGroup->physicals[0].bounds = newObjBounds;
	newGroup->physicals[0].object = newObj;
	newGroup->physicalCount = 1;

	BoundedPhysicalGroup* oldGroup = physicals;

	isLeafNode = false;
	subTrees = new NodeGroup();
	subTrees->subNodes[0] = TreeNode(oldGroup);
	subTrees->subNodes[1] = TreeNode(newGroup);
	subTrees->nodeCount = 2;*/

	NodeGroup* newNodeGroup = new NodeGroup();


	for (int i = 1; i < MAX_BRANCHES; i++) {
		newNodeGroup->subNodes[i] = TreeNode(new BoundedPhysicalGroup());
		newNodeGroup->subNodes[i].physicals->physicals[0] = std::move(physicals->physicals[i]);
		newNodeGroup->subNodes[i].physicals->physicalCount = 1;
		newNodeGroup->subNodes[i].bounds = newNodeGroup->subNodes[i].physicals->physicals[0].bounds;
	}

	newNodeGroup->subNodes[0] = TreeNode(physicals);
	newNodeGroup->subNodes[0].physicals->physicalCount = 1;
	newNodeGroup->subNodes[0].bounds = newNodeGroup->subNodes[0].physicals->physicals[0].bounds;

	newNodeGroup->nodeCount = 4;

	isLeafNode = false;
	this->subTrees = newNodeGroup;

	addToSubTrees(newObj, newObjBounds);
}

void TreeNode::addToSubTrees(Boundable * obj, const Bounds & bounds) {
	Fix<32> bestCost = computeCombinationCost(bounds, subTrees->subNodes[0].bounds);
	int bestIndex = 0;
	for (int i = 1; i < this->subTrees->nodeCount; i++) {
		Fix<32> newCost = computeCombinationCost(bounds, subTrees->subNodes[i].bounds);
		if (newCost < bestCost) {
			bestCost = newCost;
			bestIndex = i;
		}
	}
	Log::debug("bestIndex: %d", bestIndex);
	subTrees->subNodes[bestIndex].add(obj, bounds);
}

void TreeNode::add(Boundable * obj, const Bounds & bounds) {
	this->bounds = unionOfBounds(this->bounds, bounds);
	if (isLeafNode) {
		if (physicals->physicalCount < MAX_BRANCHES) {
			physicals->physicals[physicals->physicalCount++] = BoundedPhysical{ bounds, obj };
		} else {
			this->splitForNewObj(obj, bounds);
		}
	} else {
		addToSubTrees(obj, bounds);
	}
	//recalculateBounds();
}

void TreeNode::recalculateBounds() {
	if (isLeafNode) {
		this->bounds = physicals->physicals[0].bounds;
		for (int i = 1; i < physicals->physicalCount; i++) {
			this->bounds = unionOfBounds(this->bounds, physicals->physicals[i].bounds);
		}
	} else {
		this->bounds = subTrees->subNodes[0].bounds;
		for (int i = 1; i < subTrees->nodeCount; i++) {
			this->bounds = unionOfBounds(this->bounds, subTrees->subNodes[i].bounds);
		}
	}
}