#include "layer.h"


void WorldLayer::notifyPartBoundsUpdated(const Part* updatedPart, const Bounds& oldBounds) {
	tree.updateObjectBounds(updatedPart, oldBounds);
}
void WorldLayer::notifyPartGroupBoundsUpdated(const Part* mainPart, const Bounds& oldMainPartBounds) {
	tree.updateObjectGroupBounds(mainPart, oldMainPartBounds);
}

void WorldLayer::notifyPartStdMoved(Part* oldPartPtr, Part* newPartPtr) noexcept {
	bool success = tree.findAndReplaceObject(oldPartPtr, newPartPtr, newPartPtr->getBounds());
	assert(success);
}
