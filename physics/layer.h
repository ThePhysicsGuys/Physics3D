#pragma once

#include "datastructures/boundsTree.h"
#include "part.h"

class WorldLayer {
public:
	BoundsTree<Part> tree;
	WorldLayer() : tree() {
		int  i = 0;
	}
	WorldLayer(BoundsTree<Part>&& tree) : tree(std::move(tree)) {}

	void notifyPartBoundsUpdated(const Part* updatedPart, const Bounds& oldBounds);
	void notifyPartGroupBoundsUpdated(const Part* mainPart, const Bounds& oldMainPartBounds);
	/*
		When a part is std::move'd to a different location, this function is called to update any pointers
		This is something that in general should not be performed when the part is already in a world, but this function is provided for completeness
	*/
	void notifyPartStdMoved(Part* oldPartPtr, Part* newPartPtr) noexcept;
};

