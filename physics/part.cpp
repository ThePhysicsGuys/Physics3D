#include "part.h"

#include "physical.h"

#include "geometry/intersection.h"

#include "misc/validityHelper.h"

#include "catchable_assert.h"
#include <stdexcept>

namespace {
	void recalculate(Part& part) {
		part.maxRadius = part.hitbox.getMaxRadius();
	}

	void recalculateAndUpdateParent(Part& part, const Bounds& oldBounds) {
		recalculate(part);
		if (part.parent != nullptr) {
			part.parent->notifyPartPropertiesAndBoundsChanged(&part, oldBounds);
		}
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

Part::~Part() {
	if (parent != nullptr) {
		parent->removePart(this);
	}
}

Part::Part(Part&& other) :
	cframe(other.cframe),
	isTerrainPart(other.isTerrainPart),
	parent(other.parent), 
	hitbox(std::move(other.hitbox)), 
	maxRadius(other.maxRadius), 
	properties(std::move(other.properties)) {

	if(parent != nullptr) parent->notifyPartStdMoved(&other, this);

	other.parent = nullptr;
}
Part& Part::operator=(Part&& other) {
	this->cframe = other.cframe;
	this->isTerrainPart = other.isTerrainPart;
	this->parent = other.parent;
	this->hitbox = std::move(other.hitbox);
	this->maxRadius = other.maxRadius;
	this->properties = std::move(other.properties);

	if(parent != nullptr) parent->notifyPartStdMoved(&other, this);

	other.parent = nullptr;

	return *this;
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
	recalculateAndUpdateParent(*this, oldBounds);
}

void Part::setCFrame(const GlobalCFrame& newCFrame) {
	if(this->parent == nullptr) {
		this->cframe = newCFrame;
	} else {
		this->parent->setPartCFrame(this, newCFrame);
	}
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
	if(this->parent != nullptr) {
		this->parent->mainPhysical->translate(translation);
	} else {
		this->cframe.translate(translation);
	}
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
	recalculateAndUpdateParent(*this, oldBounds);
}
void Part::setHeight(double newHeight) {
	Bounds oldBounds = this->getBounds();
	this->hitbox.setHeight(newHeight);
	recalculateAndUpdateParent(*this, oldBounds);
}
void Part::setDepth(double newDepth) {
	Bounds oldBounds = this->getBounds();
	this->hitbox.setDepth(newDepth);
	recalculateAndUpdateParent(*this, oldBounds);
}


void Part::attach(Part* other, const CFrame& relativeCFrame) {
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

bool Part::isFixed() const {
	return false;
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