#include "layer.h"

#include <assert.h>


void WorldLayer::addPart(Part* newPart) {
	tree.add(newPart, newPart->getBounds());
}

void WorldLayer::removePart(Part* partToRemove) {
	tree.remove(partToRemove, partToRemove->getBounds());
}

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
