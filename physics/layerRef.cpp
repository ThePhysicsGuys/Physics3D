#include "layerRef.h"
#include "layer.h"


void LayerRef::notifyPartBoundsUpdated(const Part* updatedPart, const Bounds& oldBounds) {
	if(layer != nullptr) layer->trees[static_cast<int>(subLayer)].updateObjectBounds(updatedPart, oldBounds);
}
void LayerRef::notifyPartGroupBoundsUpdated(const Part* mainPart, const Bounds& oldMainPartBounds) {
	if(layer != nullptr) layer->trees[static_cast<int>(subLayer)].updateObjectGroupBounds(mainPart, oldMainPartBounds);
}

void LayerRef::notifyPartStdMoved(Part* oldPartPtr, Part* newPartPtr) noexcept {
	if(layer != nullptr) {
		bool success = layer->trees[static_cast<int>(subLayer)].findAndReplaceObject(oldPartPtr, newPartPtr, newPartPtr->getBounds());
		assert(success);
	}
}

