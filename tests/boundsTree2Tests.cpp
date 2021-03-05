#include "testsMain.h"

#include "../physics/datastructures/boundsTree2.h"

#include "testsMain.h"

#include "compare.h"
#include "generators.h"
#include "../physics/misc/toString.h"
#include "../physics/misc/validityHelper.h"

namespace P3D::NewBoundsTree {

TEST_CASE(testAddRemoveToBoundsTree) {
	BoundsTree<BasicBounded> tree;
	ASSERT_TRUE(isBoundsTreeValid(tree));

	constexpr int itemCount = 1000;

	std::vector<BasicBounded> itemsInTree;

	for(int i = 0; i < itemCount; i++) {
		itemsInTree.push_back(BasicBounded{generateBoundsf()});
	}
	// add all to tree
	for(int i = 0; i < itemCount; i++) {
		BasicBounded& cur = itemsInTree[i];
		ASSERT_FALSE(tree.contains(&cur));
		tree.add(&cur);
		ASSERT_TRUE(tree.contains(&cur));
	}
	ASSERT_TRUE(isBoundsTreeValid(tree));
	// assert all in tree
	for(int i = 0; i < itemCount; i++) {
		BasicBounded& cur = itemsInTree[i];
		ASSERT_TRUE(tree.contains(&cur));
	}
	ASSERT_TRUE(isBoundsTreeValid(tree));
	// remove all from tree
	for(int i = 0; i < itemCount; i++) {
		BasicBounded& cur = itemsInTree[i];
		ASSERT_TRUE(tree.contains(&cur));
		tree.remove(&cur);
		ASSERT_FALSE(tree.contains(&cur));
	}
	ASSERT_TRUE(isBoundsTreeValid(tree));
	// assert all removed from tree
	for(int i = 0; i < itemCount; i++) {
		BasicBounded& cur = itemsInTree[i];
		ASSERT_FALSE(tree.contains(&cur));
	}
	ASSERT_TRUE(isBoundsTreeValid(tree));
}

// check all groups are distinct, but internally the same
static bool groupsMatchTree(const std::vector<std::vector<BasicBounded*>>& groups, const BoundsTree<BasicBounded>& tree) {
	for(const std::vector<BasicBounded*>& groupA : groups) {
		for(const std::vector<BasicBounded*>& groupB : groups) {
			bool isSameGroup = &groupA == &groupB;
			for(const BasicBounded* objA : groupA) {
				for(const BasicBounded* objB : groupB) {
					if(tree.groupContains(objA, objB) != isSameGroup) {
						std::cout << (isSameGroup? "Objects should be in same group but aren't!" : "Objects shouldn't be in same group but are!");
						return false;
					}
				}
			}
		}
	}
	// verify that all groups in the tree match a group in the lists
	bool result = true;
	tree.forEach([&](const BasicBounded* obj) {
		size_t groupSize = tree.groupSize(obj);

		for(const std::vector<BasicBounded*>& g : groups) {
			for(BasicBounded* item : g) {
				if(obj == item) {
					if(groupSize != g.size()) {
						std::cout << "Tree item's group not same size as in groups list!\n";
						result = false;
					}
					return;
				}
			}
		}
		std::cout << "Tree item's group not in groups list!\n";
		result = false;
	});
	return result;
}

static void mergeGroups(std::vector<std::vector<BasicBounded*>>& groups, BoundsTree<BasicBounded>& tree, int groupA, int groupB) {
	BasicBounded* objA = groups[groupA][0];
	BasicBounded* objB = groups[groupB][0];
	tree.mergeGroups(objA, objB);
	groups[groupA].insert(groups[groupA].end(), groups[groupB].begin(), groups[groupB].end());
	groups[groupB] = std::move(groups.back());
	groups.pop_back();
}

TEST_CASE(testBoundsTreeGroupCreation) {
	BoundsTree<BasicBounded> tree;
	ASSERT_TRUE(isBoundsTreeValid(tree));

	constexpr int itemCount = 50;
	//constexpr int itemCount = 20;

	std::vector<BasicBounded> allItems;

	std::vector<std::vector<BasicBounded*>> groups;

	ASSERT_TRUE(isBoundsTreeValid(tree));

	for(int i = 0; i < itemCount; i++) {
		BasicBounded newObj{generateBoundsf()};
		allItems.push_back(newObj);
	}

	for(int i = 0; i < itemCount; i++) {
		BasicBounded* newObj = &allItems[i];
		int groupToAddTo = generateInt(groups.size() + 1) - 1;
		if(groupToAddTo == -1) {
			std::vector<BasicBounded*> newGroup{newObj};
			groups.push_back(std::move(newGroup));
			tree.add(newObj);
		} else {
			std::vector<BasicBounded*>& grp = groups[groupToAddTo];
			grp.push_back(newObj);
			BasicBounded* grpRep = grp[0];
			tree.addToGroup(newObj, grpRep);
			ASSERT_TRUE(tree.groupContains(newObj, grpRep));
		}
		ASSERT_TRUE(isBoundsTreeValid(tree));
	}

	ASSERT_TRUE(groupsMatchTree(groups, tree));
	ASSERT_TRUE(isBoundsTreeValid(tree));
}

TEST_CASE(testBoundsTreeGroupMerging) {
	BoundsTree<BasicBounded> tree;
	ASSERT_TRUE(isBoundsTreeValid(tree));

	constexpr int itemCount = 50;
	//constexpr int itemCount = 20;

	std::vector<BasicBounded> allItems;

	std::vector<std::vector<BasicBounded*>> groups;

	ASSERT_TRUE(isBoundsTreeValid(tree));

	for(int i = 0; i < itemCount; i++) {
		BasicBounded newObj{generateBoundsf()};
		allItems.push_back(newObj);
	}

	for(int i = 0; i < itemCount; i++) {
		BasicBounded* newObj = &allItems[i];
		int groupToAddTo = generateInt(groups.size() + 1) - 1;
		if(groupToAddTo == -1) {
			std::vector<BasicBounded*> newGroup{newObj};
			groups.push_back(std::move(newGroup));
			tree.add(newObj);
		} else {
			std::vector<BasicBounded*>& grp = groups[groupToAddTo];
			grp.push_back(newObj);
			BasicBounded* grpRep = grp[0];
			tree.addToGroup(newObj, grpRep);
			ASSERT_TRUE(tree.groupContains(newObj, grpRep));
		}
		ASSERT_TRUE(isBoundsTreeValid(tree));
	}
	
	ASSERT_TRUE(groupsMatchTree(groups, tree));
	ASSERT_TRUE(isBoundsTreeValid(tree));


	while(groups.size() > 1) {
		int mergeIdxA = generateInt(groups.size());
		int mergeIdxB;
		do {
			mergeIdxB = generateInt(groups.size());
		} while(mergeIdxA == mergeIdxB);

		mergeGroups(groups, tree, mergeIdxA, mergeIdxB);
		ASSERT_TRUE(isBoundsTreeValid(tree));
		ASSERT_TRUE(groupsMatchTree(groups, tree));
	}
}

TEST_CASE(testBoundsTreeGroupMergingSplitting) {
	std::vector<int> itemCounts{5, 15, 30, 50};
	for(int& itemCount : itemCounts) {
		//printf("\n");
		BoundsTree<BasicBounded> tree;
		ASSERT_TRUE(isBoundsTreeValid(tree));

		std::vector<BasicBounded> allItems;

		std::vector<std::vector<BasicBounded*>> groups;

		for(int i = 0; i < itemCount; i++) {
			BasicBounded newObj{generateBoundsf()};
			allItems.push_back(newObj);
		}
		tree.add(&allItems[0]);
		ASSERT_TRUE(isBoundsTreeValid(tree));
		groups.push_back(std::vector<BasicBounded*>{&allItems[0]});
		for(int i = 1; i < itemCount; i++) {
			BasicBounded* newObj = &allItems[i];
			std::vector<BasicBounded*>& grp = groups[0];
			grp.push_back(newObj);
			BasicBounded* grpRep = grp[0];
			tree.addToGroup(newObj, grpRep);
			ASSERT_TRUE(tree.groupContains(newObj, grpRep));
			ASSERT_TRUE(isBoundsTreeValid(tree));
		}

		//printf("groups.size() = %d   groups[0].size() = %d\n", (int) groups.size(), (int) groups[0].size());

		ASSERT_TRUE(groupsMatchTree(groups, tree));

		// do a whole bunch of splits and merges
		for(int iter = 0; iter < 1000; iter++) {
			int group = generateInt(groups.size());
			bool split = generateBool() || groups.size() == 1;

			if(split) {
				std::vector<BasicBounded*>& grp = groups[group];
				size_t originalSize = grp.size();
				std::vector<BasicBounded*> newGroup;

				// remove a subset of the elements from the group
				for(size_t i = 0; i < grp.size(); ) {
					if(generateBool()) {
						newGroup.push_back(grp[i]);
						grp[i] = grp.back();
						grp.pop_back();
					} else {
						i++;
					}
				}
				int newGroupSize = newGroup.size();
				int leftoverSize = grp.size();
				//printf("Split group %d into size %d-%d/%d\n", group, newGroupSize, leftoverSize, (int) originalSize);
				tree.splitGroup(newGroup.begin(), newGroup.end());
				if(grp.size() == 0) {
					grp = std::move(newGroup);
				} else {
					if(newGroupSize > 0) {
						groups.push_back(std::move(newGroup));
					}
				}
			} else { // merge
				int group2;
				do {
					group2 = generateInt(groups.size());
				} while(group == group2);

				//printf("Merged groups %d, %d\n", group, group2);
				mergeGroups(groups, tree, group, group2);
			}

			ASSERT_TRUE(isBoundsTreeValid(tree));
			ASSERT_TRUE(groupsMatchTree(groups, tree));
		}
	}
}

};
