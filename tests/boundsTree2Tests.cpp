#include "testsMain.h"

#include "../physics/datastructures/boundsTree2.h"

#include "testsMain.h"

#include "compare.h"
#include "generators.h"
#include "../physics/misc/toString.h"
#include "../physics/misc/validityHelper.h"

namespace P3D::NewBoundsTree {

TEST_CASE(testAddRemoveToBoundsTree) {
	BoundsTree tree;

	constexpr int itemCount = 1000;

	std::vector<BasicBounded> itemsInTree;

	for(int i = 0; i < itemCount; i++) {
		itemsInTree.push_back(BasicBounded{generateBoundsf()});
	}
	// add all to tree
	for(int i = 0; i < itemCount; i++) {
		BasicBounded& cur = itemsInTree[i];
		ASSERT_FALSE(tree.contains(&cur, cur.bounds));
		tree.add(&cur, cur.bounds);
		ASSERT_TRUE(tree.contains(&cur, cur.bounds));
	}
	// assert all in tree
	for(int i = 0; i < itemCount; i++) {
		BasicBounded& cur = itemsInTree[i];
		ASSERT_TRUE(tree.contains(&cur, cur.bounds));
	}
	// remove all from tree
	for(int i = 0; i < itemCount; i++) {
		BasicBounded& cur = itemsInTree[i];
		ASSERT_TRUE(tree.contains(&cur, cur.bounds));
		tree.remove(&cur, cur.bounds);
		ASSERT_FALSE(tree.contains(&cur, cur.bounds));
	}
	// assert all removed from tree
	for(int i = 0; i < itemCount; i++) {
		BasicBounded& cur = itemsInTree[i];
		ASSERT_FALSE(tree.contains(&cur, cur.bounds));
	}
}
};
