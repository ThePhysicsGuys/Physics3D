#include "layer.h"
#include "world.h"

#include "misc/validityHelper.h"
#include "misc/debug.h"
#include "misc/physicsProfiler.h"

#include <assert.h>


WorldLayer::WorldLayer(ColissionLayer* parent) : parent(parent) {}

WorldLayer::~WorldLayer() {
	for(Part& p : tree) {
		p.layer = nullptr;
	}
}

WorldLayer::WorldLayer(WorldLayer&& other) noexcept :
	tree(std::move(other.tree)),
	parent(other.parent) {

	for(Part& p : tree) {
		assert(p.layer = &other);
		p.layer = this;
	}
}
WorldLayer& WorldLayer::operator=(WorldLayer&& other) noexcept {
	std::swap(tree, other.tree);
	std::swap(parent, other.parent);

	for(Part& p : tree) {
		assert(p.layer = &other);
		p.layer = this;
	}
	for(Part& p : other.tree) {
		assert(p.layer = this);
		p.layer = &other;
	}
	return *this;
}

void WorldLayer::refresh() {
	physicsMeasure.mark(PhysicsProcess::UPDATE_TREE_BOUNDS);
	tree.recalculateBounds();
	physicsMeasure.mark(PhysicsProcess::UPDATE_TREE_STRUCTURE);
	tree.improveStructure();
}

void WorldLayer::addNode(TreeNode&& newNode) {
	tree.add(std::move(newNode));
}
void WorldLayer::addPart(Part* newPart) {
	tree.add(newPart, newPart->getBounds());
}

static TreeNode createNodeFor(MotorizedPhysical* phys, bool makeGroupHead) {
	TreeNode newNode(phys->rigidBody.mainPart, phys->rigidBody.mainPart->getBounds(), makeGroupHead);
	phys->forEachPartExceptMainPart([&newNode](Part& part) {
		newNode.addInside(TreeNode(&part, part.getBounds(), false));
	});
	return newNode;
}

void WorldLayer::addIntoGroup(Part* newPart, Part* group) {
	assert(newPart->layer == nullptr);
	assert(group->layer == this);
#ifndef NDEBUG
	treeValidCheck(tree);
#endif
	if(newPart->parent != nullptr) {
		MotorizedPhysical* mainPhys = newPart->parent->mainPhysical;
		tree.addToExistingGroup(createNodeFor(mainPhys, false), group, group->getBounds());
		mainPhys->forEachPart([this](Part& p) {p.layer = this; });
#ifndef NDEBUG
		treeValidCheck(tree);
#endif
	} else {
		tree.addToExistingGroup(TreeNode(newPart, newPart->getBounds()), group, group->getBounds());
		newPart->layer = this;
#ifndef NDEBUG
		treeValidCheck(tree);
#endif
	}
}

void WorldLayer::moveOutOfGroup(Part* part) {
	this->tree.moveOutOfGroup(part, part->getBounds());
}

void WorldLayer::removePart(Part* partToRemove) {
	tree.remove(partToRemove, partToRemove->getBounds());
	parent->world->onPartRemoved(partToRemove);
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

int WorldLayer::getID() const {
	return (parent->getID() * ColissionLayer::NUMBER_OF_SUBLAYERS) + (this - parent->subLayers);
}
WorldLayer* getLayerByID(std::vector<ColissionLayer>& knownLayers, int id) {
	return &knownLayers[id / ColissionLayer::NUMBER_OF_SUBLAYERS].subLayers[id % ColissionLayer::NUMBER_OF_SUBLAYERS];
}
const WorldLayer* getLayerByID(const std::vector<ColissionLayer>& knownLayers, int id) {
	return &knownLayers[id / ColissionLayer::NUMBER_OF_SUBLAYERS].subLayers[id % ColissionLayer::NUMBER_OF_SUBLAYERS];
}
int getMaxLayerID(const std::vector<ColissionLayer>& knownLayers) {
	return knownLayers.size() * ColissionLayer::NUMBER_OF_SUBLAYERS;
}

int ColissionLayer::getID() const {
	return this - &world->layers[0];
}

ColissionLayer::ColissionLayer() : world(nullptr), collidesInternally(true), subLayers{WorldLayer(this), WorldLayer(this)} {}
ColissionLayer::ColissionLayer(WorldPrototype* world, bool collidesInternally) : world(world), collidesInternally(collidesInternally), subLayers{WorldLayer(this), WorldLayer(this)} {}

ColissionLayer::ColissionLayer(ColissionLayer&& other) noexcept : world(other.world), collidesInternally(other.collidesInternally), subLayers{std::move(other.subLayers[0]), std::move(other.subLayers[1])} {
	other.world = nullptr;

	for(WorldLayer& l : subLayers) {
		l.parent = this;
	}
}
ColissionLayer& ColissionLayer::operator=(ColissionLayer&& other) noexcept {
	std::swap(this->world, other.world);
	std::swap(this->subLayers, other.subLayers);
	std::swap(this->collidesInternally, other.collidesInternally);

	for(WorldLayer& l : subLayers) {
		l.parent = this;
	}
	for(WorldLayer& l : other.subLayers) {
		l.parent = &other;
	}
	return *this;
}

void ColissionLayer::refresh() {
	subLayers[FREE_PARTS_LAYER].refresh();
}



static bool boundsSphereEarlyEnd(const DiagonalMat3& scale, const Vec3& sphereCenter, double sphereRadius) {
	return std::abs(sphereCenter.x) > scale[0] + sphereRadius || std::abs(sphereCenter.y) > scale[1] + sphereRadius || std::abs(sphereCenter.z) > scale[2] + sphereRadius;
}

static bool runColissionPreTests(const Part& p1, const Part& p2) {
	Vec3 offset = p1.getPosition() - p2.getPosition();
	if(isLongerThan(offset, p1.maxRadius + p2.maxRadius)) {
		intersectionStatistics.addToTally(IntersectionResult::PART_DISTANCE_REJECT, 1);
		return false;
	}
	if(boundsSphereEarlyEnd(p1.hitbox.scale, p1.getCFrame().globalToLocal(p2.getPosition()), p2.maxRadius)) {
		intersectionStatistics.addToTally(IntersectionResult::PART_BOUNDS_REJECT, 1);
		return false;
	}
	if(boundsSphereEarlyEnd(p2.hitbox.scale, p2.getCFrame().globalToLocal(p1.getPosition()), p1.maxRadius)) {
		intersectionStatistics.addToTally(IntersectionResult::PART_BOUNDS_REJECT, 1);
		return false;
	}

	return true;
}

static void recursiveFindColissionsBetween(std::vector<Colission>& colissions, const TreeNode& first, const TreeNode& second) {
	if(!intersects(first.bounds, second.bounds)) return;

	if(first.isLeafNode() && second.isLeafNode()) {
		Part* p1 = static_cast<Part*>(first.object);
		Part* p2 = static_cast<Part*>(second.object);
		if(runColissionPreTests(*p1, *p2)) {
			colissions.push_back(Colission{p1, p2, Position(), Vec3()});
		}
	} else {
		bool preferFirst = computeCost(first.bounds) <= computeCost(second.bounds);
		if(preferFirst && !first.isLeafNode() || second.isLeafNode()) {
			// split first

			for(const TreeNode& node : first) {
				recursiveFindColissionsBetween(colissions, node, second);
			}
		} else {
			// split second

			for(const TreeNode& node : second) {
				recursiveFindColissionsBetween(colissions, first, node);
			}
		}
	}
}
static void recursiveFindColissionsInternal(std::vector<Colission>& colissions, const TreeNode& trunkNode) {
	// within the same node
	if(trunkNode.isLeafNode() || trunkNode.isGroupHead)
		return;

	for(int i = 0; i < trunkNode.nodeCount; i++) {
		const TreeNode& A = trunkNode[i];
		recursiveFindColissionsInternal(colissions, A);
		for(int j = i + 1; j < trunkNode.nodeCount; j++) {
			const TreeNode& B = trunkNode[j];
			recursiveFindColissionsBetween(colissions, A, B);
		}
	}
}

void ColissionLayer::getInternalColissions(ColissionBuffer& curColissions) const {
	recursiveFindColissionsInternal(curColissions.freePartColissions, subLayers[0].tree.rootNode);
	recursiveFindColissionsBetween(curColissions.freeTerrainColissions, subLayers[0].tree.rootNode, subLayers[1].tree.rootNode);
}
void getColissionsBetween(const ColissionLayer& a, const ColissionLayer& b, ColissionBuffer& curColissions) {
	recursiveFindColissionsBetween(curColissions.freePartColissions, a.subLayers[0].tree.rootNode, b.subLayers[0].tree.rootNode);
	recursiveFindColissionsBetween(curColissions.freeTerrainColissions, a.subLayers[0].tree.rootNode, b.subLayers[1].tree.rootNode);
	recursiveFindColissionsBetween(curColissions.freeTerrainColissions, b.subLayers[0].tree.rootNode, a.subLayers[1].tree.rootNode);
}
