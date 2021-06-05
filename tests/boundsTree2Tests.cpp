#include "testsMain.h"

#include <Physics3D/boundstree/boundsTree.h>

#include "testsMain.h"

#include "compare.h"
#include "generators.h"
#include <Physics3D/misc/toString.h>
#include <Physics3D/misc/validityHelper.h>

#include <vector>
#include <set>

using namespace P3D;

static void shuffleTreeRecursive(TreeTrunk& curTrunk, int curTrunkSize) {
	for(int iter = 0; iter < (curTrunkSize - 1) * curTrunkSize; iter++) {
		int index1 = generateInt(curTrunkSize);
		int index2;
		do {
			index2 = generateInt(curTrunkSize);
		} while(index1 == index2);

		BoundsTemplate<float> tmpBounds = curTrunk.getBoundsOfSubNode(index1);
		TreeNodeRef tmpNode = std::move(curTrunk.subNodes[index1]);

		curTrunk.setSubNode(index1, std::move(curTrunk.subNodes[index2]), curTrunk.getBoundsOfSubNode(index2));
		curTrunk.setSubNode(index2, std::move(tmpNode), tmpBounds);
	}

	for(int i = 0; i < curTrunkSize; i++) {
		TreeNodeRef& subNode = curTrunk.subNodes[i];
		if(subNode.isTrunkNode()) {
			shuffleTreeRecursive(subNode.asTrunk(), subNode.getTrunkSize());
		}
	}
}
static void shuffleTree(BoundsTreePrototype& tree) {
	std::pair<TreeTrunk&, int> baseTrunk = tree.getBaseTrunk();
	shuffleTreeRecursive(baseTrunk.first, baseTrunk.second);
}
template<typename Boundable>
static void shuffleTree(BoundsTree<Boundable>& tree) {
	shuffleTree(tree.getPrototype());
}

static BoundsTemplate<float> generateBoundsTreeBounds() {
	float x = generateFloat(-100.0f, 100.0f);
	float y = generateFloat(-100.0f, 100.0f);
	float z = generateFloat(-100.0f, 100.0f);

	float w = generateFloat(0.2f, 50.0f);
	float h = generateFloat(0.2f, 50.0f);
	float d = generateFloat(0.2f, 50.0f);

	return BoundsTemplate<float>(PositionTemplate<float>(x - w, y - h, z - d), PositionTemplate<float>(x + w, y + h, z + d));
}

static std::vector<BasicBounded> generateBoundsTreeItems(int size) {
	std::vector<BasicBounded> allItems;

	for(int i = 0; i < size; i++) {
		allItems.push_back(BasicBounded{generateBoundsTreeBounds()});
	}

	return allItems;
}

TEST_CASE(testAddRemoveToBoundsTree) {
	BoundsTree<BasicBounded> tree;
	ASSERT_TRUE(isBoundsTreeValid(tree));

	constexpr int itemCount = 1000;

	std::vector<BasicBounded> itemsInTree = generateBoundsTreeItems(itemCount);

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
						std::cout << (isSameGroup ? "Objects should be in same group but aren't!" : "Objects shouldn't be in same group but are!");
						return false;
					}
				}
			}
		}
	}
	// verify that all groups in the tree match a group in the lists
	bool result = true;
	tree.forEach([&](const BasicBounded& obj) {
		size_t groupSize = tree.groupSize(&obj);

		for(const std::vector<BasicBounded*>& g : groups) {
			for(BasicBounded* item : g) {
				if(&obj == item) {
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

	std::vector<BasicBounded> allItems = generateBoundsTreeItems(itemCount);

	std::vector<std::vector<BasicBounded*>> groups;

	ASSERT_TRUE(isBoundsTreeValid(tree));

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

static std::vector<std::vector<BasicBounded*>> createGroups(BoundsTree<BasicBounded>& tree, std::vector<BasicBounded>& allItems) {
	assert(tree.isEmpty());

	std::vector<std::vector<BasicBounded*>> groups;

	// add to groups
	for(int i = 0; i < allItems.size() / 2; i++) {
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
		}
	}

	// also add some loose objects
	for(int i = allItems.size() / 2; i < allItems.size(); i++) {
		BasicBounded* newObj = &allItems[i];
		std::vector<BasicBounded*> newGroup{newObj};
		groups.push_back(std::move(newGroup));
		tree.add(newObj);
	}

	shuffleTree(tree);

	return groups;
}

TEST_CASE(testBoundsTreeGroupMerging) {
	BoundsTree<BasicBounded> tree;
	ASSERT_TRUE(isBoundsTreeValid(tree));

	constexpr int itemCount = 50;
	//constexpr int itemCount = 20;

	std::vector<BasicBounded> allItems = generateBoundsTreeItems(itemCount);

	std::vector<std::vector<BasicBounded*>> groups = createGroups(tree, allItems);

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

TEST_CASE_SLOW(testBoundsTreeGroupMergingSplitting) {
	std::vector<int> itemCounts{5, 15, 30, 50};
	for(int& itemCount : itemCounts) {
		//printf("\n");
		BoundsTree<BasicBounded> tree;
		ASSERT_TRUE(isBoundsTreeValid(tree));

		std::vector<BasicBounded> allItems = generateBoundsTreeItems(itemCount);

		std::vector<std::vector<BasicBounded*>> groups;

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

TEST_CASE(testForEachColission) {
	BoundsTree<BasicBounded> tree;

	constexpr int itemCount = 100;

	std::vector<BasicBounded> allItems = generateBoundsTreeItems(itemCount);

	std::vector<std::vector<BasicBounded*>> groups = createGroups(tree, allItems);

	std::set<std::pair<BasicBounded*, BasicBounded*>> foundColissions;

	ASSERT_TRUE(isBoundsTreeValid(tree));

	tree.forEachColission([&](BasicBounded* a, BasicBounded* b) {
		ASSERT_STRICT(a != b);
		if(b < a) std::swap(a, b);
		std::pair<BasicBounded*, BasicBounded*> col(a, b);
		ASSERT_FALSE(foundColissions.find(col) != foundColissions.end()); // no duplicate colissions
		foundColissions.insert(col);
	});

	for(size_t i = 0; i < groups.size(); i++) {
		std::vector<BasicBounded*>& grp = groups[i];

		for(size_t aIndex = 0; aIndex < grp.size(); aIndex++) {
			BasicBounded* a = grp[aIndex];
			for(size_t bIndex = aIndex + 1; bIndex < grp.size(); bIndex++) {
				BasicBounded* b = grp[bIndex];
				if(a > b) std::swap(a, b);
				std::pair<BasicBounded*, BasicBounded*> col(a, b);

				ASSERT_FALSE(foundColissions.find(col) != foundColissions.end());
			}
		}
	}

	for(size_t i = 0; i < groups.size(); i++) {
		std::vector<BasicBounded*>& groupA = groups[i];

		for(size_t j = i + 1; j < groups.size(); j++) {
			std::vector<BasicBounded*>& groupB = groups[j];

			for(int ai = 0; ai < groupA.size(); ai++) {
				for(int bi = 0; bi < groupB.size(); bi++) {
					BasicBounded* a = groupA[ai];
					BasicBounded* b = groupB[bi];
					if(a > b) std::swap(a, b);
					std::pair<BasicBounded*, BasicBounded*> col(a, b);

					bool wasFound = foundColissions.find(col) != foundColissions.end();

					bool shouldBeFound = intersects(a->bounds, b->bounds);

					ASSERT_STRICT(wasFound == shouldBeFound);
				}
			}
		}
	}
}

TEST_CASE(testForEachColissionBetween) {
	BoundsTree<BasicBounded> tree1;
	BoundsTree<BasicBounded> tree2;

	constexpr int itemCount = 100;

	std::vector<BasicBounded> allItems1 = generateBoundsTreeItems(itemCount);
	std::vector<BasicBounded> allItems2 = generateBoundsTreeItems(itemCount);

	std::vector<std::vector<BasicBounded*>> groups1 = createGroups(tree1, allItems1);
	std::vector<std::vector<BasicBounded*>> groups2 = createGroups(tree2, allItems2);

	std::set<std::pair<BasicBounded*, BasicBounded*>> foundColissions;

	tree1.forEachColissionWith(tree2, [&](BasicBounded* a, BasicBounded* b) {
		std::pair<BasicBounded*, BasicBounded*> col(a, b);
		ASSERT_FALSE(foundColissions.find(col) != foundColissions.end()); // no duplicate colissions
		foundColissions.insert(col);
	});

	for(size_t i = 0; i < groups1.size(); i++) {
		std::vector<BasicBounded*>& groupA = groups1[i];

		for(size_t j = i + 1; j < groups2.size(); j++) {
			std::vector<BasicBounded*>& groupB = groups2[i];

			for(BasicBounded* a : groupA) {
				for(BasicBounded* b : groupB) {
					std::pair<BasicBounded*, BasicBounded*> col(a, b);

					bool wasFound = foundColissions.find(col) != foundColissions.end();

					bool shouldBeFound = intersects(a->bounds, b->bounds);

					ASSERT_STRICT(wasFound == shouldBeFound);
				}
			}
		}
	}
}

TEST_CASE(testUpdatePartBounds) {
	BoundsTree<BasicBounded> tree;

	constexpr int itemCount = 100;

	std::vector<BasicBounded> allItems = generateBoundsTreeItems(itemCount);

	std::vector<std::vector<BasicBounded*>> groups = createGroups(tree, allItems);

	ASSERT_TRUE(isBoundsTreeValid(tree));

	for(int iter = 0; iter < 500; iter++) {
		BasicBounded& selectedItem = allItems[generateSize_t(allItems.size())];
		BoundsTemplate<float> oldBounds = selectedItem.getBounds();
		selectedItem.bounds = generateBoundsTreeBounds();

		tree.updateObjectBounds(&selectedItem, oldBounds);
		ASSERT_TRUE(tree.contains(&selectedItem));
		ASSERT_TRUE(isBoundsTreeValid(tree));
	}
}

TEST_CASE(testUpdateGroupBounds) {
	BoundsTree<BasicBounded> tree;

	constexpr int itemCount = 10;

	std::vector<BasicBounded> allItems = generateBoundsTreeItems(itemCount);

	std::vector<std::vector<BasicBounded*>> groups = createGroups(tree, allItems);

	ASSERT_TRUE(isBoundsTreeValid(tree));

	for(int iter = 0; iter < 500; iter++) {
		std::vector<BasicBounded*>& selectedGroup = groups[generateSize_t(groups.size())];
		size_t selectedGroupSize = selectedGroup.size();
		BasicBounded& selectedItem = *selectedGroup[generateSize_t(selectedGroupSize)];
		BoundsTemplate<float> oldBounds = selectedItem.getBounds();
		for(BasicBounded* item : selectedGroup) {
			item->bounds = generateBoundsTreeBounds();
		}

		tree.updateObjectGroupBounds(&selectedItem, oldBounds);
		ASSERT_TRUE(tree.contains(&selectedItem));
		ASSERT_TRUE(isBoundsTreeValid(tree));
	}
}

TEST_CASE(testImproveStructureValidity) {
	BoundsTree<BasicBounded> tree;

	constexpr int itemCount = 100;

	std::vector<BasicBounded> allItems = generateBoundsTreeItems(itemCount);

	std::vector<std::vector<BasicBounded*>> groups = createGroups(tree, allItems);

	ASSERT_TRUE(groupsMatchTree(groups, tree));
	ASSERT_TRUE(isBoundsTreeValid(tree));

	for(int iter = 0; iter < 10; iter++) {
		for(BasicBounded& bb : allItems) {
			bb.bounds = generateBoundsTreeBounds();
		}
		tree.recalculateBounds();

		ASSERT_TRUE(groupsMatchTree(groups, tree));
		ASSERT_TRUE(isBoundsTreeValid(tree));

		for(int i = 0; i < 5; i++) {
			tree.improveStructure();
			ASSERT_TRUE(groupsMatchTree(groups, tree));
			ASSERT_TRUE(isBoundsTreeValid(tree));
		}
	}
}
