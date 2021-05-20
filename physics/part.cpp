#include "part.h"

#include "physical.h"

#include "geometry/intersection.h"

#include "misc/validityHelper.h"

#include "catchable_assert.h"
#include <stdexcept>
#include <set>
#include <utility>


#include "layer.h"

namespace P3D {
namespace {
void recalculate(Part* part) {
	part->maxRadius = part->hitbox.getMaxRadius();
}

void recalculateAndUpdateParent(Part* part, const Bounds& oldBounds) {
	recalculate(part);
	if(part->parent != nullptr) {
		part->parent->notifyPartPropertiesChanged(part);
	}
	if(part->layer != nullptr) part->layer->notifyPartBoundsUpdated(part, oldBounds);
}
};

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

	if (parent != nullptr) parent->notifyPartStdMoved(&other, this);
	if (layer != nullptr) layer->notifyPartStdMoved(&other, this);

	other.parent = nullptr;
	other.layer = nullptr;
}
Part& Part::operator=(Part&& other) noexcept {
	this->cframe = other.cframe;
	this->layer = other.layer;
	this->parent = other.parent;
	this->hitbox = std::move(other.hitbox);
	this->maxRadius = other.maxRadius;
	this->properties = std::move(other.properties);

	if (parent != nullptr) parent->notifyPartStdMoved(&other, this);
	if (layer != nullptr) layer->notifyPartStdMoved(&other, this);

	other.parent = nullptr;
	other.layer = nullptr;

	return *this;
}

WorldPrototype* Part::getWorld() {
	if (layer == nullptr)
		return nullptr;
	
	return layer->parent->world;
}

Part::~Part() {
	this->removeFromWorld();
}

int Part::getLayerID() const {
	return layer->getID();
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

BoundsTemplate<float> Part::getBounds() const {
	BoundingBox boundsOfHitbox = this->hitbox.getBounds(this->cframe.getRotation());

	assert(isVecValid(boundsOfHitbox.min));
	assert(isVecValid(boundsOfHitbox.max));

	return BoundsTemplate<float>(boundsOfHitbox + getPosition());
}

void Part::scale(double scaleX, double scaleY, double scaleZ) {
	Bounds oldBounds = this->getBounds();
	this->hitbox = this->hitbox.scaled(scaleX, scaleY, scaleZ);
	recalculateAndUpdateParent(this, oldBounds);
}

void Part::setScale(const DiagonalMat3& scale) {
	Bounds oldBounds = this->getBounds();
	this->hitbox.scale = scale;
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

Vec3 Part::getVelocity() const {
	return this->getMotion().getVelocity();
}
Vec3 Part::getAngularVelocity() const {
	return this->getMotion().getAngularVelocity();
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

void Part::setVelocity(Vec3 velocity) {
	Vec3 oldVel = this->getVelocity();
	parent->mainPhysical->motionOfCenterOfMass.translation.translation[0] += (velocity - oldVel);
}
void Part::setAngularVelocity(Vec3 angularVelocity) {
	Vec3 oldAngularVel = this->getAngularVelocity();
	parent->mainPhysical->motionOfCenterOfMass.rotation.rotation[0] += (angularVelocity - oldAngularVel);
}
void Part::setMotion(Vec3 velocity, Vec3 angularVelocity) {
	setAngularVelocity(angularVelocity); // angular velocity must come first, as it affects the velocity that setVelocity() uses
	setVelocity(velocity);
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

double Part::getFriction() {
	return this->properties.friction;
}

double Part::getDensity() {
	return this->properties.density;
}

double Part::getBouncyness() {
	return this->properties.bouncyness;
}

Vec3 Part::getConveyorEffect() {
	return this->properties.conveyorEffect;
}

void Part::setMass(double mass) {
	setDensity(mass / hitbox.getVolume());
	// TODO update necessary?
}

void Part::setFriction(double friction) {
	this->properties.friction = friction;
	// TODO update necessary?
}

void Part::setDensity(double density) {
	this->properties.density = density;
	// TODO update necessary?
}

void Part::setBouncyness(double bouncyness) {
	this->properties.bouncyness = bouncyness;
	// TODO update necessary?
}

void Part::setConveyorEffect(const Vec3& conveyorEffect) {
	this->properties.conveyorEffect = conveyorEffect;
	// TODO update necessary?
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

static void mergePartLayers(Part* first, Part* second, const std::vector<FoundLayerRepresentative>& layersOfFirst, const std::vector<FoundLayerRepresentative>& layersOfSecond) {
	for(const FoundLayerRepresentative& l1 : layersOfFirst) {
		for(const FoundLayerRepresentative& l2 : layersOfSecond) {
			if(l1.layer == l2.layer) {
				l1.layer->mergeGroups(l1.part, l2.part);
				break;
			}
		}
	}
}

static void updateGroupBounds(std::vector<FoundLayerRepresentative>& layers, std::vector<Bounds>& bounds) {
	for(size_t i = 0; i < layers.size(); i++) {
		layers[i].layer->notifyPartGroupBoundsUpdated(layers[i].part, bounds[i]);
	}
}

static std::vector<Part*> getAllPartsInPhysical(Part* rep) {
	std::vector<Part*> result;
	if(rep->parent != nullptr) {
		rep->parent->mainPhysical->forEachPart([&result](Part& part) {
			result.push_back(&part);
		});
	} else {
		result.push_back(rep);
	}
	return result;
}

static void addAllToGroupLayer(Part* layerOwner, const std::vector<Part*>& partsToAdd) {
	layerOwner->layer->addAllToGroup(partsToAdd.begin(), partsToAdd.end(), layerOwner);
	for(Part* p : partsToAdd) {
		p->layer = layerOwner->layer;
	}
}

template<typename PhysicalMergeFunc>
static void mergeLayersAround(Part* first, Part* second, PhysicalMergeFunc mergeFunc) {
	if(second->layer != nullptr) {
		std::vector<FoundLayerRepresentative> layersOfSecond = findAllLayersIn(second);
		std::vector<Bounds> boundsOfSecondParts(layersOfSecond.size());
		for(size_t i = 0; i < layersOfSecond.size(); i++) {
			boundsOfSecondParts[i] = layersOfSecond[i].part->getBounds();
		}

		if(first->layer != nullptr) {
			std::vector<FoundLayerRepresentative> layersOfFirst = findAllLayersIn(first);
		
			mergeFunc();
			updateGroupBounds(layersOfSecond, boundsOfSecondParts);

			mergePartLayers(first, second, layersOfFirst, layersOfSecond);
		} else {
			std::vector<Part*> partsInFirst = getAllPartsInPhysical(first);
			
			mergeFunc();
			updateGroupBounds(layersOfSecond, boundsOfSecondParts);
			
			addAllToGroupLayer(second, partsInFirst);
		}
	} else {
		if(first->layer != nullptr) {
			std::vector<Part*> partsInSecond = getAllPartsInPhysical(second);

			mergeFunc();

			addAllToGroupLayer(first, partsInSecond);
		} else {
			mergeFunc();
		}
	}
}

void Part::attach(Part* other, const CFrame& relativeCFrame) {
	mergeLayersAround(this, other, [&]() {
		if(this->parent == nullptr) {
			this->parent = new MotorizedPhysical(this);
			this->parent->attachPart(other, relativeCFrame);
		} else {
			this->parent->attachPart(other, this->transformCFrameToParent(relativeCFrame));
		}
	});
}

void Part::attach(Part* other, HardConstraint* constraint, const CFrame& attachToThis, const CFrame& attachToThat) {
	mergeLayersAround(this, other, [&]() {
		this->ensureHasParent();
		this->parent->attachPart(other, constraint, attachToThis, attachToThat);
	});
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
};