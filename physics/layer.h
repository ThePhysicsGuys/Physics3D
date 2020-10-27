#pragma once

#include "datastructures/boundsTree.h"
#include "part.h"

class WorldPrototype;

class WorldLayer {
public:
	BoundsTree<Part> tree;
	WorldPrototype* world;

	explicit WorldLayer(WorldPrototype* world);

	void addPart(Part* newPart);
	void removePart(Part* partToRemove);
	void addIntoGroup(Part* newPart, Part* group);
	template<typename PartIterBegin, typename PartIterEnd>
	void addAllToGroup(PartIterBegin begin, PartIterEnd end, Part* group) {
		tree.addAllToExistingGroup(begin, end, group);
	}
	//void addIntoGroup(MotorizedPhysical* newPhys, Part* group);

	void notifyPartBoundsUpdated(const Part* updatedPart, const Bounds& oldBounds);
	void notifyPartGroupBoundsUpdated(const Part* mainPart, const Bounds& oldMainPartBounds);
	/*
		When a part is std::move'd to a different location, this function is called to update any pointers
		This is something that in general should not be performed when the part is already in a world, but this function is provided for completeness
	*/
	void notifyPartStdMoved(Part* oldPartPtr, Part* newPartPtr) noexcept;

	void mergeGroupsOf(Part* first, Part* second);
	void moveIntoGroup(Part* partToMove, Part* group);
	void moveOutOfGroup(Part* part);
	void joinPartsIntoNewGroup(Part* p1, Part* p2);

	template<typename PartIterBegin, typename PartIterEnd>
	void moveAllOutOfGroup(PartIterBegin begin, PartIterEnd end) {
		tree.moveAllOutOfGroup(begin, end);
	}
};

