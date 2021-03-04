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
	ASSERT_TRUE(isBoundsTreeValid(tree));
	// assert all in tree
	for(int i = 0; i < itemCount; i++) {
		BasicBounded& cur = itemsInTree[i];
		ASSERT_TRUE(tree.contains(&cur, cur.bounds));
	}
	ASSERT_TRUE(isBoundsTreeValid(tree));
	// remove all from tree
	for(int i = 0; i < itemCount; i++) {
		BasicBounded& cur = itemsInTree[i];
		ASSERT_TRUE(tree.contains(&cur, cur.bounds));
		tree.remove(&cur, cur.bounds);
		ASSERT_FALSE(tree.contains(&cur, cur.bounds));
	}
	ASSERT_TRUE(isBoundsTreeValid(tree));
	// assert all removed from tree
	for(int i = 0; i < itemCount; i++) {
		BasicBounded& cur = itemsInTree[i];
		ASSERT_FALSE(tree.contains(&cur, cur.bounds));
	}
	ASSERT_TRUE(isBoundsTreeValid(tree));
}

TEST_CASE(testGroupsOfBoundsTree) {
	BoundsTree tree;

	constexpr int itemCount = 50;

	std::vector<BasicBounded> allItems;

	std::vector<std::vector<BasicBounded*>> groups;

	ASSERT_TRUE(isBoundsTreeValid(tree));

	for(int i = 0; i < itemCount; i++) {
		BasicBounded newObj{generateBoundsf()};
		allItems.push_back(newObj);
	}

	for(int i = 0; i < itemCount; i++) {
		BasicBounded& newObj = allItems[i];
		int groupToAddTo = generateInt(groups.size() + 1) - 1;
		if(groupToAddTo == -1) {
			std::vector<BasicBounded*> newGroup{&newObj};
			groups.push_back(std::move(newGroup));
			tree.add(&newObj, newObj.bounds);
		} else {
			std::vector<BasicBounded*>& grp = groups[groupToAddTo];
			grp.push_back(&newObj);
			BasicBounded* grpRep = grp[0];
			tree.addToGroup(&newObj, newObj.bounds, grpRep, grpRep->bounds);
			ASSERT_TRUE(tree.groupContains(&newObj, newObj.bounds, grpRep, grpRep->bounds));
		}
		ASSERT_TRUE(isBoundsTreeValid(tree));
	}
	
	// check all groups are distinct, but internally the same
	for(std::vector<BasicBounded*>& groupA : groups) {
		for(std::vector<BasicBounded*>& groupB : groups) {
			bool isSameGroup = &groupA == &groupB;
			for(const BasicBounded* objA : groupA) {
				for(const BasicBounded* objB : groupB) {
					ASSERT_STRICT(tree.groupContains(objA, objA->bounds, objB, objB->bounds) == isSameGroup);
				}
			}
		}
	}
	ASSERT_TRUE(isBoundsTreeValid(tree));


	while(groups.size() > 1) {
		int mergeIdxA = generateInt(groups.size());
		int mergeIdxB;
		do {
			mergeIdxB = generateInt(groups.size());
		} while(mergeIdxA == mergeIdxB);
		
		BasicBounded* objA = groups[mergeIdxA][0];
		BasicBounded* objB = groups[mergeIdxB][0];
		ASSERT_TRUE(isBoundsTreeValid(tree));
		tree.mergeGroups(objA, objA->bounds, objB, objB->bounds);
		ASSERT_TRUE(isBoundsTreeValid(tree));
		groups[mergeIdxA].insert(groups[mergeIdxA].end(), groups[mergeIdxB].begin(), groups[mergeIdxB].end());
		groups[mergeIdxB] = std::move(groups.back());
		groups.pop_back();

		// check all groups are distinct, but internally the same
		for(std::vector<BasicBounded*>& groupA : groups) {
			for(std::vector<BasicBounded*>& groupB : groups) {
				bool isSameGroup = &groupA == &groupB;
				for(const BasicBounded* objA : groupA) {
					for(const BasicBounded* objB : groupB) {
						ASSERT_STRICT(tree.groupContains(objA, objA->bounds, objB, objB->bounds) == isSameGroup);
					}
				}
			}
		}
		ASSERT_TRUE(isBoundsTreeValid(tree));
	}
}

TEST_CASE(testBoundsTreeGroupSplitting) {
	BoundsTree tree;

	constexpr int itemCount = 1000;

	std::vector<BasicBounded> allItems;

	std::vector<std::vector<BasicBounded*>> groups;

	for(int i = 0; i < itemCount; i++) {
		BasicBounded newObj{generateBoundsf()};
		allItems.push_back(newObj);
	}

	for(int i = 0; i < itemCount; i++) {
		BasicBounded& newObj = allItems[i];
		int groupToAddTo = generateInt(groups.size() + 1) - 1;
		if(groupToAddTo == -1) {
			std::vector<BasicBounded*> newGroup{&newObj};
			groups.push_back(std::move(newGroup));
			tree.add(&newObj, newObj.bounds);
		} else {
			std::vector<BasicBounded*>& grp = groups[groupToAddTo];
			grp.push_back(&newObj);
			BasicBounded* grpRep = grp[0];
			tree.addToGroup(&newObj, newObj.bounds, grpRep, grpRep->bounds);
			ASSERT_TRUE(tree.groupContains(&newObj, newObj.bounds, grpRep, grpRep->bounds));
		}
	}

	// check all groups are distinct, but internally the same
	for(std::vector<BasicBounded*>& groupA : groups) {
		for(std::vector<BasicBounded*>& groupB : groups) {
			bool isSameGroup = &groupA == &groupB;
			for(const BasicBounded* objA : groupA) {
				for(const BasicBounded* objB : groupB) {
					ASSERT_STRICT(tree.groupContains(objA, objA->bounds, objB, objB->bounds) == isSameGroup);
				}
			}
		}
	}

	throw "TODO";
}

};
