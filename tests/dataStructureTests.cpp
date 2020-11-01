#include "testsMain.h"

#include "compare.h"
#include "generators.h"
#include "../physics/misc/toString.h"
#include "../physics/misc/validityHelper.h"

#include "../physics/datastructures/boundsTree.h"

TEST_CASE(testBoundsTreeGenerationValid) {
	for(int iter = 0; iter < 1000; iter++) {
		BoundsTree<BasicBounded> tree = generateBoundsTree();
		treeValidCheck(tree);
		ASSERT_TRUE(true);
	}
}

TEST_CASE(testAddObjToBoundsTree) {
	for(int iter = 0; iter < 1000; iter++) {
		BoundsTree<BasicBounded> tree = generateBoundsTree();
		for(int jiter = 0; jiter < 10; jiter++) {
			BasicBounded* newObj = new BasicBounded{generateBounds()};
			ASSERT_FALSE(tree.contains(newObj));
			tree.add(newObj);
			ASSERT_TRUE(tree.contains(newObj));
			treeValidCheck(tree);
		}
	}
}
TEST_CASE(testRemoveObjFromBoundsTree) {
	for(int iter = 0; iter < 1000; iter++) {
		BoundsTree<BasicBounded> tree = generateBoundsTree();
		while(!tree.isEmpty()) {
			BasicBounded* selectedObj = getRandomObjectFromTree(tree);
			ASSERT_TRUE(tree.contains(selectedObj));
			tree.remove(selectedObj);
			ASSERT_FALSE(tree.contains(selectedObj));
			treeValidCheck(tree);
		}
	}
}
static bool areGroupedCorrectly(const BoundsTree<BasicBounded>& tree, const std::vector<BasicBounded*>& lstA, const std::vector<BasicBounded*>& lstB, bool sameGroup) {
	for(BasicBounded* a : lstA) {
		for(BasicBounded* b : lstB) {
			if(tree.areInSameGroup(a, b) != sameGroup) {
				return false;
			}
		}
	}
	return true;
}
TEST_CASE(testMergeGroups) {
	int successCount = 0;
	while(successCount < 100) {
		BoundsTree<BasicBounded> tree = generateFilledBoundsTree();
		BasicBounded* first = getRandomObjectFromTree(tree);
		BasicBounded* second = getRandomObjectFromTree(tree);

		if(!tree.areInSameGroup(first, second)) {
			std::vector<BasicBounded*> firstGroup;
			auto iter = tree.iterAllInGroup(first);
			for(BasicBounded& firstGroupItem : iter) {
				firstGroup.push_back(&firstGroupItem);
			}
			std::vector<BasicBounded*> secondGroup;
			for(BasicBounded& firstGroupItem : tree.iterAllInGroup(first)) {
				firstGroup.push_back(&firstGroupItem);
			}

			ASSERT_TRUE(areGroupedCorrectly(tree, firstGroup, firstGroup, true));
			ASSERT_TRUE(areGroupedCorrectly(tree, secondGroup, secondGroup, true));
			ASSERT_TRUE(areGroupedCorrectly(tree, firstGroup, secondGroup, false));

			tree.mergeGroupsOf(first, second);

			ASSERT_TRUE(areGroupedCorrectly(tree, firstGroup, firstGroup, true));
			ASSERT_TRUE(areGroupedCorrectly(tree, secondGroup, secondGroup, true));
			ASSERT_TRUE(areGroupedCorrectly(tree, firstGroup, secondGroup, true));

			treeValidCheck(tree);
			successCount++;
		}
	}
}
