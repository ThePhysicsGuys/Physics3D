#include "part.h"

#include "physical.h"

#include "geometry/intersection.h"

#include "misc/validityHelper.h"

#include "catchable_assert.h"
#include <stdexcept>
#include <set>
#include <utility>


#include "layer.h"

namespace {
	void recalculate(Part* part) {
		part->maxRadius = part->hitbox.getMaxRadius();
	}

	void recalculateAndUpdateParent(Part* part, const Bounds& oldBounds) {
		recalculate(part);
		if(part->parent != nullptr) {
			part->parent->notifyPartPropertiesChanged(part);
		}
		if(part->layer != nullptr) part->layer->notifyPartGroupBoundsUpdated(part, oldBounds);
	}
}

Part::Part(const Shape& shape, const GlobalCFrame& position, const PartProperties& properties) : 
	hitbox(shape), properties(properties), maxRadius(shape.getMaxRadius()), cframe(position) {
}

Part::Part(const Shape& shape, Part& attachTo, const CFrame& attach, const PartProperties& properties) : 
	hitbox(shape), properties(properties), maxRadius(shape.getMaxRadius()), cframe(attachTo.cframe.localToGlobal(attach)) {
	attachTo.attach(this, attach);
}

Part::Part(const Shape& shape, Part& attachTo, HardConstraint* constraint, const CFrame& attachToParent, const CFrame& attachToThis, const PartProperties& properties) : 
	hitbox(shape), properties(properties), maxRadius(shape.getMaxRadius()), cframe(attachTo.getCFrame().localToGlobal(attachToParent.localToGlobal(constraint->getRelativeCFrame()).localToGlobal(attachToThis))) {
	attachTo.attach(this, constraint, attachToParent, attachToThis);
}

Part::Part(Part&& other) noexcept :
	cframe(other.cframe),
	layer(other.layer),
	parent(other.parent), 
	hitbox(std::move(other.hitbox)), 
	maxRadius(other.maxRadius), 
	properties(std::move(other.properties)) {

	if(parent != nullptr) parent->notifyPartStdMoved(&other, this);
	if(layer != nullptr) layer->notifyPartStdMoved(&other, this);

	other.parent = nullptr;
}
Part& Part::operator=(Part&& other) noexcept {
	this->cframe = other.cframe;
	this->layer = other.layer;
	this->parent = other.parent;
	this->hitbox = std::move(other.hitbox);
	this->maxRadius = other.maxRadius;
	this->properties = std::move(other.properties);

	if(parent != nullptr) parent->notifyPartStdMoved(&other, this);
	if(layer != nullptr) layer->notifyPartStdMoved(&other, this);

	other.parent = nullptr;

	return *this;
}

Part::~Part() {
	this->removeFromWorld();
}

void Part::removeFromWorld() {
	if(this->parent) this->parent->removePart(this);
	if(this->layer) this->layer->removePart(this);
}

PartIntersection Part::intersects(const Part& other) const {
	CFrame relativeTransform = this->cframe.globalToLocal(other.cframe);
	std::optional<Intersection> result = intersectsTransformed(this->hitbox, other.hitbox, relativeTransform);
	if(result) {
		Position intersection = this->cframe.localToGlobal(result.value().intersection);
		Vec3 exitVector = this->cframe.localToRelative(result.value().exitVector);


		catchable_assert(isVecValid(exitVector));


		return PartIntersection(intersection, exitVector);
	}
	return PartIntersection();
}

BoundingBox Part::getLocalBounds() const {
	Vec3 v = Vec3(this->hitbox.scale[0], this->hitbox.scale[1], this->hitbox.scale[2]);
	return BoundingBox(-v, v);
}

Bounds Part::getBounds() const {
	BoundingBox boundsOfHitbox = this->hitbox.getBounds(this->cframe.getRotation());
	
	assert(isVecValid(boundsOfHitbox.min));
	assert(isVecValid(boundsOfHitbox.max));

	return boundsOfHitbox + getPosition();
}

void Part::scale(double scaleX, double scaleY, double scaleZ) {
	Bounds oldBounds = this->getBounds();
	this->hitbox = this->hitbox.scaled(scaleX, scaleY, scaleZ);
	recalculateAndUpdateParent(this, oldBounds);
}

void Part::setCFrame(const GlobalCFrame& newCFrame) {
	Bounds oldBounds = this->getBounds();
	if(this->parent == nullptr) {
		this->cframe = newCFrame;
	} else {
		this->parent->setPartCFrame(this, newCFrame);
	}
	if(this->layer != nullptr) this->layer->notifyPartGroupBoundsUpdated(this, oldBounds);
}

Motion Part::getMotion() const {
	if(parent == nullptr) return Motion();
	Motion parentsMotion = parent->getMotion();
	if(this->isMainPart()) {
		return parentsMotion;
	} else {
		Vec3 offset = parent->rigidBody.getAttachFor(this).attachment.getPosition();
		return parentsMotion.getMotionOfPoint(offset);
	}
}

void Part::translate(Vec3 translation) {
	Bounds oldBounds = this->getBounds();
	if(this->parent != nullptr) {
		this->parent->mainPhysical->translate(translation);
	} else {
		this->cframe += translation;
	}
	if(this->layer != nullptr) this->layer->notifyPartGroupBoundsUpdated(this, oldBounds);
}

double Part::getWidth() const {
	return this->hitbox.getWidth();
}
double Part::getHeight() const {
	return this->hitbox.getHeight();
}
double Part::getDepth() const {
	return this->hitbox.getDepth();
}

void Part::setWidth(double newWidth) {
	Bounds oldBounds = this->getBounds();
	this->hitbox.setWidth(newWidth);
	recalculateAndUpdateParent(this, oldBounds);
}
void Part::setHeight(double newHeight) {
	Bounds oldBounds = this->getBounds();
	this->hitbox.setHeight(newHeight);
	recalculateAndUpdateParent(this, oldBounds);
}
void Part::setDepth(double newDepth) {
	Bounds oldBounds = this->getBounds();
	this->hitbox.setDepth(newDepth);
	recalculateAndUpdateParent(this, oldBounds);
}


void Part::applyForce(Vec3 relativeOrigin, Vec3 force) {
	Vec3 originOffset = this->getPosition() - this->parent->mainPhysical->getPosition();
	this->parent->mainPhysical->applyForce(originOffset + relativeOrigin, force);
}
void Part::applyForceAtCenterOfMass(Vec3 force) {
	Vec3 originOffset = this->getCenterOfMass() - this->parent->mainPhysical->getPosition();
	this->parent->mainPhysical->applyForce(originOffset, force);
}
void Part::applyMoment(Vec3 moment) {
	this->parent->mainPhysical->applyMoment(moment);
}


struct PairLess {
	inline bool operator()(std::pair<WorldLayer*, Part*> first, std::pair<WorldLayer*, Part*> snd) const {
		return first.first < snd.first;
	}
};

static std::set<std::pair<WorldLayer*, Part*>, PairLess> getLayersInPhysical(MotorizedPhysical* phys) {
	std::set<std::pair<WorldLayer*, Part*>, PairLess> result;

	phys->forEachPart([&result](Part& part) {
		result.insert(std::make_pair(part.layer, &part));
	});

	return result;
}

static Part* getPartOfLayerInPhysical(MotorizedPhysical* phys, const WorldLayer* layer) {
	return phys->findFirst([layer](Part& part) {return part.layer == layer; });
}

static void mergePhysicalLayers(MotorizedPhysical* first, MotorizedPhysical* second) {
	std::set<std::pair<WorldLayer*, Part*>, PairLess> layersOfFirst = getLayersInPhysical(first);
	std::set<std::pair<WorldLayer*, Part*>, PairLess> layersOfSecond = getLayersInPhysical(second);

	for(std::pair<WorldLayer*, Part*> lSecond : layersOfSecond) {
		auto found = layersOfFirst.find(lSecond);
		if(found != layersOfFirst.end()) {
			// merge layers
			std::pair<WorldLayer*, Part*> lFirst = *found;

			assert(lFirst.first == lSecond.first);

			lFirst.first->mergeGroupsOf(lFirst.second, lSecond.second);
		}
	}
}


static void mergePartLayers(Part* first, Part* second) {
	if(first->parent != nullptr) {
		if(second->parent != nullptr) {
			mergePhysicalLayers(first->parent->mainPhysical, second->parent->mainPhysical);
		} else {
			Part* foundPartInLayer = getPartOfLayerInPhysical(first->parent->mainPhysical, second->layer);
			// add second part into first
			if(foundPartInLayer != nullptr) {
				second->layer->moveIntoGroup(second, foundPartInLayer);
			}
		}
	} else {
		if(second->parent != nullptr) {
			Part* foundPartInLayer = getPartOfLayerInPhysical(second->parent->mainPhysical, first->layer);
			// add second part into first
			if(foundPartInLayer != nullptr) {
				first->layer->moveIntoGroup(first, foundPartInLayer);
			}
		} else {
			if(first->layer == second->layer) {
				first->layer->joinPartsIntoNewGroup(first, second);
			}
		}
	}
}

static void moveWholePhysIntoLayer(Part* part, Part* layerOwner) {
	assert(layerOwner->layer != nullptr);
	assert(part->layer == nullptr);

	layerOwner->layer->addIntoGroup(part, layerOwner);
}

static void joinLayers(Part* main, Part* other) {
	if(main->layer != nullptr) {
		if(other->layer != nullptr) {
			mergePartLayers(main, other);
		} else {
			moveWholePhysIntoLayer(other, main);
		}
	} else {
		if(other->layer != nullptr) {
			moveWholePhysIntoLayer(main, other);
		}
	}
}

void Part::attach(Part* other, const CFrame& relativeCFrame) {
	joinLayers(this, other);
	if(this->parent == nullptr) {
		this->parent = new MotorizedPhysical(this);
		this->parent->attachPart(other, relativeCFrame);
	} else {
		this->parent->attachPart(other, this->transformCFrameToParent(relativeCFrame));
	}
}

void Part::attach(Part* other, HardConstraint* constraint, const CFrame& attachToThis, const CFrame& attachToThat) {
	this->ensureHasParent();
	this->parent->attachPart(other, constraint, attachToThis, attachToThat);
}

void Part::detach() {
	if(this->parent == nullptr) throw std::logic_error("No physical to detach from!");
	this->parent->detachPart(this);
	if(this->layer != nullptr) {
		this->layer->moveOutOfGroup(this);
	}
}

void Part::ensureHasParent() {
	if(this->parent == nullptr) {
		this->parent = new MotorizedPhysical(this);
	}
}

CFrame Part::transformCFrameToParent(const CFrame& cframeRelativeToPart) {
	if(this->isMainPart()) {
		return cframeRelativeToPart;
	} else {
		return this->parent->rigidBody.getAttachFor(this).attachment.localToGlobal(cframeRelativeToPart);
	}
}

bool Part::isMainPart() const {
	return this->parent == nullptr || this->parent->rigidBody.mainPart == this;
}

void Part::makeMainPart() {
	if(!this->isMainPart()) {
		this->parent->makeMainPart(this);
	}
}

bool Part::isValid() const {
	assert(std::isfinite(hitbox.getVolume()));
	assert(std::isfinite(maxRadius));
	assert(std::isfinite(properties.density));
	assert(std::isfinite(properties.friction));
	assert(std::isfinite(properties.bouncyness));
	assert(isVecValid(properties.conveyorEffect));

	return true;
}