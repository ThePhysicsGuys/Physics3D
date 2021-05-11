#include "benchmark.h"

#include "../physics/datastructures/boundsTree.h"
#include "../physics/datastructures/boundsTreeOld.h"

namespace P3D::OldBoundsTree {
#define BENCH_TREE_BRANCH_FACTOR 4
static void fillTreeNodeRecursive(TreeNode& node, BasicBounded* curItemList, size_t numberOfItems) {
	if(numberOfItems == 1) {
		node = TreeNode(curItemList, curItemList->getBounds());
	} else {
		node = TreeNode::withEmptySubNodes();
		node.nodeCount = BENCH_TREE_BRANCH_FACTOR;
		size_t numberOfItemsForSubNode = numberOfItems / BENCH_TREE_BRANCH_FACTOR;
		for(int i = 0; i < BENCH_TREE_BRANCH_FACTOR; i++) {
			fillTreeNodeRecursive(node[i], curItemList + i * numberOfItemsForSubNode, numberOfItemsForSubNode);
		}
	}
}

#define BENCH_TREE_NODECOUNT 1 << 18
struct FindInBoundsTreeBenchmark : public Benchmark {
	FindInBoundsTreeBenchmark() : Benchmark("findInBoundsTree") {}

	BoundsTree<BasicBounded> tree;
	BasicBounded objects[BENCH_TREE_NODECOUNT];

	int total = 0;

	virtual void init() override {
		Position curPos(0, 0, 0);
		Vec3Fix delta(0.1, 0.1, 0.1);
		Vec3Fix diag(0.13, 0.13, 0.13);
		for(BasicBounded& b : objects) {
			b.bounds = Bounds(curPos, curPos + diag);
			curPos += delta;
		}
		fillTreeNodeRecursive(tree.rootNode, objects, BENCH_TREE_NODECOUNT);
		tree.recalculateBounds();
	}
	virtual void run() override {
		for(int i = 0; i < 100; i++) {
			for(const BasicBounded& obj : objects) {
				auto stack = tree.find(&obj, obj.bounds);
				total += stack.getSize();
			}
		}
	}
} boundsTreeBenchmark;
};
