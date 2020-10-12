#include "layer.h"
#include "world.h"

#include <assert.h>


WorldLayer::WorldLayer(WorldPrototype* world) : world(world) {}

void WorldLayer::addPart(Part* newPart) {
	tree.add(newPart, newPart->getBounds());
}

static TreeNode createNodeFor(MotorizedPhysical* phys) {
	TreeNode newNode(phys->rigidBody.mainPart, phys->rigidBody.mainPart->getBounds(), true);
	phys->forEachPartExceptMainPart([&newNode](Part& part) {
		newNode.addInside(TreeNode(&part, part.getBounds(), false));
	});
	return newNode;
}

void WorldLayer::addIntoGroup(Part* newPart, Part* group) {
	assert(newPart->layer == nullptr);
	assert(group->layer == this);
	if(newPart->parent != nullptr) {
		MotorizedPhysical* mainPhys = newPart->parent->mainPhysical;
		tree.addToExistingGroup(createNodeFor(mainPhys), group, group->getBounds());
		mainPhys->forEachPart([this](Part& p) {p.layer = this; });
	} else {
		tree.addToExistingGroup(TreeNode(newPart, newPart->getBounds()), group, group->getBounds());
		newPart->layer = this;
	}
}

void WorldLayer::moveOutOfGroup(Part* part) {
	this->tree.moveOutOfGroup(part, part->getBounds());
}

void WorldLayer::removePart(Part* partToRemove) {
	tree.remove(partToRemove, partToRemove->getBounds());
	world->onPartRemoved(partToRemove);
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

void WorldLayer::mergeGroupsOf(Part* first, Part* second) {
	this->tree.mergeGroupsOf(first, first->getBounds(), second, second->getBounds());
}

// TODO can be optimized, this only needs to move the single partToMove node
void WorldLayer::moveIntoGroup(Part* partToMove, Part* group) {
	this->tree.mergeGroupsOf(partToMove, partToMove->getBounds(), group, group->getBounds());
}

// TODO can be optimized, this only needs to move the single part nodes
void WorldLayer::joinPartsIntoNewGroup(Part* p1, Part* p2) {
	this->tree.mergeGroupsOf(p1, p1->getBounds(), p2, p2->getBounds());
}

