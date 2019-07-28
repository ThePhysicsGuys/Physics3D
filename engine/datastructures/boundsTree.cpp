#include "boundsTree.h"

long long computeCost(const Bounds & bounds) {
	Vec3Fix d = bounds.getDiagonal();
	return (d.x + d.y + d.z).value;
}

/*
Computes a metric to show the cost of combining two boundingboxes
*/
inline long long computeCombinationCost(const Bounds& newBox, const Bounds& expandingBox) {
	Bounds combinedBounds = unionOfBounds(newBox, expandingBox);
	return computeCost(combinedBounds);
}

TreeNode::~TreeNode() {
	if (!isLeafNode()) {
		delete[] subTrees;
	}
}

void TreeNode::addToSubTrees(TreeNode&& newNode) {
	long long bestCost = computeCombinationCost(newNode.bounds, subTrees[0].bounds);
	int bestIndex = 0;
	for (int i = 1; i < this->nodeCount; i++) {
		long long newCost = computeCombinationCost(newNode.bounds, subTrees[i].bounds);
		if (newCost < bestCost) {
			bestCost = newCost;
			bestIndex = i;
		}
	}
	Log::debug("bestIndex: %d", bestIndex);
	subTrees[bestIndex].add(std::move(newNode));
}

void TreeNode::add(TreeNode&& newNode) {
	if (isLeafNode()) {
		TreeNode* newNodes = new TreeNode[MAX_BRANCHES];

		newNodes[0].object = this->object;
		newNodes[0].bounds = this->bounds;

		new(&newNodes[1]) TreeNode(std::move(newNode));

		this->nodeCount = 2;
		this->subTrees = newNodes;
	}else{
		if (nodeCount != MAX_BRANCHES) {
			new(&subTrees[nodeCount++]) TreeNode(std::move(newNode));
		} else {
			addToSubTrees(std::move(newNode));
		}
	}
	this->bounds = unionOfBounds(this->bounds, newNode.bounds);
}

void TreeNode::recalculateBoundsFromSubBounds() {
	this->bounds = subTrees[0].bounds;
	for (int i = 1; i < nodeCount; i++) {
		this->bounds = unionOfBounds(this->bounds, subTrees[i].bounds);
	}
}

void TreeNode::recalculateBounds(bool strictBounds) {
	if (isLeafNode()) {
		this->bounds = strictBounds ? object->getStrictBounds() : object->getLooseBounds();
	} else {
		recalculateBoundsFromSubBounds();
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

void transferObject(TreeNode& from, TreeNode& to, size_t index){
	to.add(std::move(from.subTrees[index]));
	new(&from.subTrees[index]) TreeNode(std::move(from.subTrees[--from.nodeCount]));
}

void exchangeObjects(TreeNode& first, TreeNode& second) {
	// send objects from first to second

	// compute bounds of this without the given node
	Bounds boundsWithout = first.subTrees[1].bounds;
	for (size_t i = 2; i < first.nodeCount; i++)
		boundsWithout = unionOfBounds(boundsWithout, first.subTrees[i].bounds);

	Bounds boundsWithSecond = unionOfBounds(second.bounds, first.subTrees[0].bounds);

	long long gain = computeCost(first.bounds) - computeCost(boundsWithout);
	long long loss = computeCost(boundsWithSecond) - computeCost(second.bounds);

	if (gain > loss) {
		transferObject(first, second, 0);
		return;
	}

	Bounds boundsUpToNow = first.subTrees[0].bounds;
	for (size_t i = 1; i < first.nodeCount; i++) {
		Bounds boundWithout = boundsUpToNow;
		for (size_t j = i + 1; j < first.nodeCount; j++)
			boundWithout = unionOfBounds(boundWithout, first.subTrees[j].bounds);

		Bounds boundsWithSecond = unionOfBounds(second.bounds, first.subTrees[i].bounds);

		long long gain = computeCost(first.bounds) - computeCost(boundWithout);
		long long loss = computeCost(boundsWithSecond) - computeCost(second.bounds);

		if (gain > loss) {
			transferObject(first, second, i);
			return;
		}
	}
}

Bounds computeBoundsOfList(const TreeNode* const * list, size_t count) {
	Bounds bounds = list[0]->bounds;

	for (size_t i = 1; i < count; i++) {
		bounds = unionOfBounds(bounds, list[i]->bounds);
	}
	return bounds;
}

Bounds computeBoundsOfList(const TreeNode* list, size_t count) {
	Bounds bounds = list[0].bounds;

	for (size_t i = 1; i < count; i++) {
		bounds = unionOfBounds(bounds, list[i].bounds);
	}
	return bounds;
}

struct NodePermutation {
	TreeNode* permutationA[MAX_BRANCHES];
	TreeNode* permutationB[MAX_BRANCHES];
	size_t countA = 0;
	size_t countB = 0;

	inline void pushA(TreeNode* newNode) { permutationA[countA++] = newNode; }
	inline void pushB(TreeNode* newNode) { permutationB[countB++] = newNode; }
	inline void pushAN(TreeNode* const * newNodes, size_t count) { for(int i = 0; i < count; i++) permutationA[countA++] = newNodes[i]; }
	inline void pushBN(TreeNode* const * newNodes, size_t count) { for(int i = 0; i < count; i++) permutationB[countB++] = newNodes[i]; }
	inline void popA() { countA--; }
	inline void popB() { countB--; }
	inline void popAN(int amount) { countA -= amount; }
	inline void popBN(int amount) { countB -= amount; }
	inline void popAToB() { permutationB[countB++] = permutationA[--countA]; }
	inline void popBToA() { permutationA[countA++] = permutationB[--countB]; }
	inline void replaceA(TreeNode* newNode) { permutationA[countA - 1] = newNode; }
	inline void replaceB(TreeNode* newNode) { permutationB[countB - 1] = newNode; }
	inline void replaceAPushToB(TreeNode* newNode) { permutationB[countB++] = permutationA[countA - 1]; permutationA[countA - 1] = newNode; }
	inline void replaceBPushToA(TreeNode* newNode) { permutationA[countA++] = permutationB[countB - 1]; permutationB[countB - 1] = newNode; }

	inline Bounds getBoundsA() const { return computeBoundsOfList(permutationA, countA); }
	inline Bounds getBoundsB() const { return computeBoundsOfList(permutationB, countB); }

	inline void swap() { std::swap(countA, countB); std::swap(permutationA, permutationB); }
};

typedef FixedLocalBuffer<TreeNode*, 2*MAX_BRANCHES> Buf;

FixedLocalBuffer<TreeNode*, 2 * MAX_BRANCHES> nodesToList(TreeNode& first, TreeNode& second) {
	FixedLocalBuffer<TreeNode*, 2 * MAX_BRANCHES> allNodes;

	TreeNode* topNodes[2] = { &first, &second };

	for (TreeNode* node : topNodes) {
		if (node->isLeafNode()) {
			allNodes.add(node);
		} else {
			for (int i = 0; i < node->nodeCount; i++) {
				allNodes.add(&node->subTrees[i]);
			}
		}
	}
	return allNodes;
}


long long computeCost(const NodePermutation& perm) {
	return computeCost(perm.getBoundsA()) + computeCost(perm.getBoundsB());
}

void updateBestPermutationIfNeeded(long long& bestCost, NodePermutation& bestPermutation, NodePermutation& currentPermutation) {
	long long cost = computeCost(currentPermutation);
	if (cost < bestCost) {
		bestPermutation = currentPermutation;
		bestCost = cost;
	}
}

void recursiveFindBestCombination(long long& bestCost, NodePermutation& bestPermutation, NodePermutation& currentPermutation, TreeNode*const* candidates, size_t size) {
	if (size == 0) { // all nodes have been placed
		updateBestPermutationIfNeeded(bestCost, bestPermutation, currentPermutation);
	} else { // some nodes still left to place
		currentPermutation.pushA(candidates[0]);
		if (currentPermutation.countA == MAX_BRANCHES) {
			for (int i = 1; i < size; i++) {
				currentPermutation.pushB(candidates[i]);
			}
			updateBestPermutationIfNeeded(bestCost, bestPermutation, currentPermutation);
			currentPermutation.popBN(size-1);
		} else {
			recursiveFindBestCombination(bestCost, bestPermutation, currentPermutation, candidates + 1, size - 1);
		}
		currentPermutation.popAToB();
		if (currentPermutation.countB == MAX_BRANCHES) {
			for (int i = 1; i < size; i++) {
				currentPermutation.pushA(candidates[i]);
			}
			updateBestPermutationIfNeeded(bestCost, bestPermutation, currentPermutation);
			currentPermutation.popAN(size-1);
		} else {
			recursiveFindBestCombination(bestCost, bestPermutation, currentPermutation, candidates + 1, size - 1);
		}
		currentPermutation.popB();
	}
}

/*
	This function tries all permutations of the given nodes, and finds which arrangement results in the smallest bounds when split into two groups
*/
NodePermutation findBestPermutation(const FixedLocalBuffer<TreeNode*, 2 * MAX_BRANCHES>& allNodes, long long initialBestCost) {
	NodePermutation bestPermutation;
	NodePermutation currentPermutation;
	
	long long bestCost = initialBestCost;

	/*
	A B...
	AB C...
	ABC D...
	ABCD EFGH
	*/
	currentPermutation.pushB(allNodes[0]);
	for (int i = 0; i < allNodes.size - 1; i++) {
		currentPermutation.replaceBPushToA(allNodes[i + 1]);  // A B    AB C    ABC D     ABCD E
		if (currentPermutation.countA == MAX_BRANCHES) {
			for (int j = MAX_BRANCHES + 1; j < allNodes.size; j++) {
				currentPermutation.pushB(allNodes[j]);
			}
			updateBestPermutationIfNeeded(bestCost, bestPermutation, currentPermutation);
			break;
		}
		recursiveFindBestCombination(bestCost, bestPermutation, currentPermutation, allNodes.buf + i + 2, allNodes.size - i - 2);
	}

	return bestPermutation;
}

void fillNodePairWithPermutation(TreeNode& first, TreeNode& second, NodePermutation& bestPermutation) {
	if (bestPermutation.countA == 1) bestPermutation.swap(); // make sure that the leafnode is always permutationB

	TreeNode * availableGroups[2];
	int existingGroups = 0;
	if (!first.isLeafNode()) { availableGroups[existingGroups++] = first.subTrees; }
	if (!second.isLeafNode()) { availableGroups[existingGroups++] = second.subTrees; }

	TreeNode nodesCopyA[MAX_BRANCHES] = {};
	for (int i = 0; i < bestPermutation.countA; i++) { nodesCopyA[i] = std::move(*bestPermutation.permutationA[i]); }
	TreeNode nodesCopyB[MAX_BRANCHES] = {};
	for (int i = 0; i < bestPermutation.countB; i++) { nodesCopyB[i] = std::move(*bestPermutation.permutationB[i]); }

	int groupsNeeded = 1 + (bestPermutation.countB != 1);

	if (existingGroups < groupsNeeded) {// tops one extra group to be made
		availableGroups[existingGroups++] = new TreeNode[MAX_BRANCHES];
	} else if (existingGroups > groupsNeeded) {
		delete[] availableGroups[--existingGroups];
	}

	first.subTrees = availableGroups[0];
	for (int i = 0; i < bestPermutation.countA; i++) first.subTrees[i] = std::move(nodesCopyA[i]);
	first.nodeCount = bestPermutation.countA;

	if (bestPermutation.countB != 1) {
		second.subTrees = availableGroups[1];
		for (int i = 0; i < bestPermutation.countB; i++) second.subTrees[i] = std::move(nodesCopyB[i]);
		second.nodeCount = bestPermutation.countB;
	} else {
		new(&second) TreeNode(std::move(nodesCopyB[0]));
	}

	first.recalculateBoundsFromSubBounds();
	if (!second.isLeafNode()) second.recalculateBoundsFromSubBounds();
}

/*
	This function tries all permutations of the subnodes of first and second, and finds which arrangement results in the smallest bounds
*/
void optimizeNodePair(TreeNode& first, TreeNode& second) {
	if (first.isLeafNode() && second.isLeafNode())
		return;

	long long bestCost = computeCost(first.bounds) + computeCost(second.bounds);

	NodePermutation bestPermutation = findBestPermutation(nodesToList(first, second), bestCost);

	if(bestPermutation.countA != 0)
		fillNodePairWithPermutation(first, second, bestPermutation);
}

void TreeNode::improveStructure() {
	if (!isLeafNode()) {
		for (int i = 0; i < nodeCount; i++) subTrees[i].improveStructure();
		for (int i = 0; i < nodeCount - 1; i++) {
			TreeNode& A = subTrees[i];
			for (int j = i + 1; j < nodeCount; j++) {
				TreeNode& B = subTrees[j];
				if (intersects(A.bounds, B.bounds)) {
					optimizeNodePair(A, B);
				}
			}
		}
	}
}
