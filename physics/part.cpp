#include "part.h"

#include "physical.h"

#include "geometry/intersection.h"

#include "misc/validityHelper.h"

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

Part::Part(const Shape& shape, const GlobalCFrame& position, const PartProperties& properties)
	: hitbox(shape), cframe(position), properties(properties) {
	recalculate(*this);
}

Part::Part(const Shape& shape, Part& attachTo, const CFrame& attach, const PartProperties& properties)
	: hitbox(shape), cframe(attachTo.cframe.localToGlobal(attach)), properties(properties) {
	attachTo.attach(this, attach);
	recalculate(*this);
}

Part::~Part() {
	if (parent != nullptr) {
		parent->detachPart(this, false);
	}
}

Part::Part(Part&& other) :
	isTerrainPart(other.isTerrainPart),
	parent(other.parent), 
	hitbox(std::move(other.hitbox)), 
	maxRadius(other.maxRadius), 
	properties(std::move(other.properties)) {

	if(parent != nullptr) parent->notifyPartStdMoved(&other, this);

	other.parent = nullptr;
}
Part& Part::operator=(Part&& other) {
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


		assert(isVecValid(exitVector));


		return PartIntersection(intersection, exitVector);
	}
	return PartIntersection();
}

BoundingBox Part::getLocalBounds() const {
	Vec3 v = Vec3(this->hitbox.scale[0], this->hitbox.scale[1], this->hitbox.scale[2]);
	return BoundingBox(-v, v);
}

Bounds Part::getStrictBounds() const {
	BoundingBox boundsOfHitbox = this->hitbox.getBounds(this->cframe.getRotation());
	
	assert(isVecValid(boundsOfHitbox.min));
	assert(isVecValid(boundsOfHitbox.max));

	return boundsOfHitbox + getPosition();
}

void Part::scale(double scaleX, double scaleY, double scaleZ) {
	Bounds oldBounds = this->getStrictBounds();
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
	Vec3 offset = this->cframe.getPosition() - parent->rigidBody.getCenterOfMass();
	return parentsMotion.getMotionOfPoint(offset);
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
	Bounds oldBounds = this->getStrictBounds();
	this->hitbox.setWidth(newWidth);
	recalculateAndUpdateParent(*this, oldBounds);
}
void Part::setHeight(double newHeight) {
	Bounds oldBounds = this->getStrictBounds();
	this->hitbox.setHeight(newHeight);
	recalculateAndUpdateParent(*this, oldBounds);
}
void Part::setDepth(double newDepth) {
	Bounds oldBounds = this->getStrictBounds();
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
	if(this->parent == nullptr) throw "No physical to detach from!";
	this->parent->detachPart(this, true);
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
	assert(isfinite(hitbox.getVolume()));
	assert(isfinite(maxRadius));
	assert(isfinite(properties.density));
	assert(isfinite(properties.friction));
	assert(isfinite(properties.bouncyness));
	assert(isVecValid(properties.conveyorEffect));

	return true;
}