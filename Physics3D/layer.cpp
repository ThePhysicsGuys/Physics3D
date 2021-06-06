#include "layer.h"
#include "world.h"

#include "misc/validityHelper.h"
#include "misc/debug.h"
#include "misc/physicsProfiler.h"

#include <assert.h>

namespace P3D {
WorldLayer::WorldLayer(ColissionLayer* parent) : parent(parent) {}

WorldLayer::~WorldLayer() {
	tree.forEach([](Part& p) {
		p.layer = nullptr;
	});
}

WorldLayer::WorldLayer(WorldLayer&& other) noexcept :
	tree(std::move(other.tree)),
	parent(other.parent) {

	tree.forEach([this, &other](Part& p) {
		assert(p.layer = &other);
		p.layer = this;
	});
}
WorldLayer& WorldLayer::operator=(WorldLayer&& other) noexcept {
	std::swap(tree, other.tree);
	std::swap(parent, other.parent);

	tree.forEach([this, &other](Part& p) {
		assert(p.layer = &other);
		p.layer = this;
	});
	other.tree.forEach([this, &other](Part& p) {
		assert(p.layer = this);
		p.layer = &other;
	});
	return *this;
}

void WorldLayer::refresh() {
	physicsMeasure.mark(PhysicsProcess::UPDATE_TREE_BOUNDS);
	tree.recalculateBounds();
	physicsMeasure.mark(PhysicsProcess::UPDATE_TREE_STRUCTURE);
	tree.improveStructure();
}

void WorldLayer::addPart(Part* newPart) {
	tree.add(newPart);
}

static void addMotorPhysToGroup(BoundsTree<Part>& tree, MotorizedPhysical* phys, Part* group) {
	auto base = tree.getPrototype().getBaseTrunk();
	TrunkAllocator& alloc = tree.getPrototype().getAllocator();
	modifyGroupRecursive(alloc, base.first, base.second, group, group->getBounds(), [&](TreeNodeRef& groupNode, const BoundsTemplate<float>& groupNodeBounds) {
		Part* mp = phys->getMainPart();
		if(groupNode.isLeafNode()) {
			TreeTrunk* newTrunk = alloc.allocTrunk();
			newTrunk->setSubNode(0, std::move(groupNode), groupNodeBounds);
			newTrunk->setSubNode(1, TreeNodeRef(mp), mp->getBounds());
			groupNode = TreeNodeRef(newTrunk, 2, true);
		} else {
			addRecursive(alloc, groupNode.asTrunk(), groupNode.getTrunkSize(), TreeNodeRef(mp), mp->getBounds());
		}
		TreeTrunk& curTrunk = groupNode.asTrunk();
		int curTrunkSize = groupNode.getTrunkSize();
		phys->forEachPartExceptMainPart([&](Part& part) {
			curTrunkSize = addRecursive(alloc, curTrunk, curTrunkSize, TreeNodeRef(&part), part.getBounds());
		});
		groupNode.setTrunkSize(curTrunkSize);
		return TrunkSIMDHelperFallback::getTotalBounds(curTrunk, curTrunkSize);
	});
}

void WorldLayer::addIntoGroup(Part* newPart, Part* group) {
	assert(newPart->layer == nullptr);
	assert(group->layer == this);

	if(newPart->parent != nullptr) {
		MotorizedPhysical* mainPhys = newPart->parent->mainPhysical;
		addMotorPhysToGroup(tree, mainPhys, group);
		mainPhys->forEachPart([this](Part& p) {p.layer = this; });
	} else {
		tree.addToGroup(newPart, group);
		newPart->layer = this;
	}
}

void WorldLayer::moveOutOfGroup(Part* part) {
	this->tree.moveOutOfGroup(part);
}

void WorldLayer::removePart(Part* partToRemove) {
	tree.remove(partToRemove);
	parent->world->onPartRemoved(partToRemove);
}

void WorldLayer::notifyPartBoundsUpdated(const Part* updatedPart, const Bounds& oldBounds) {
	tree.updateObjectBounds(updatedPart, oldBounds);
}
void WorldLayer::notifyPartGroupBoundsUpdated(const Part* mainPart, const Bounds& oldMainPartBounds) {
	tree.updateObjectGroupBounds(mainPart, oldMainPartBounds);
}

void WorldLayer::notifyPartStdMoved(Part* oldPartPtr, Part* newPartPtr) noexcept {
	tree.findAndReplaceObject(oldPartPtr, newPartPtr, newPartPtr->getBounds());
}

void WorldLayer::mergeGroups(Part* first, Part* second) {
	this->tree.mergeGroups(first, second);
}

// TODO can be optimized, this only needs to move the single partToMove node
void WorldLayer::moveIntoGroup(Part* partToMove, Part* group) {
	this->tree.mergeGroups(partToMove, group);
}

// TODO can be optimized, this only needs to move the single part nodes
void WorldLayer::joinPartsIntoNewGroup(Part* p1, Part* p2) {
	this->tree.mergeGroups(p1, p2);
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

static void findColissionsBetween(std::vector<Colission>& colissions, const BoundsTree<Part>& treeA, const BoundsTree<Part>& treeB) {
	treeA.forEachColissionWith(treeB, [&colissions](Part* a, Part* b) {
		colissions.push_back(Colission{a, b});
	});
}
static void findColissionsInternal(std::vector<Colission>& colissions, const BoundsTree<Part>& tree) {
	tree.forEachColission([&colissions](Part* a, Part* b) {
		colissions.push_back(Colission{a, b});
	});
}

void ColissionLayer::getInternalColissions(ColissionBuffer& curColissions) const {
	findColissionsInternal(curColissions.freePartColissions, subLayers[0].tree);
	findColissionsBetween(curColissions.freeTerrainColissions, subLayers[0].tree, subLayers[1].tree);
}
void getColissionsBetween(const ColissionLayer& a, const ColissionLayer& b, ColissionBuffer& curColissions) {
	findColissionsBetween(curColissions.freePartColissions, a.subLayers[0].tree, b.subLayers[0].tree);
	findColissionsBetween(curColissions.freeTerrainColissions, a.subLayers[0].tree, b.subLayers[1].tree);
	findColissionsBetween(curColissions.freeTerrainColissions, b.subLayers[0].tree, a.subLayers[1].tree);
}
};