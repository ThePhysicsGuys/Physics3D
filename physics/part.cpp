#include "part.h"

#include "physical.h"

#include "math/globalTransform.h"

#include "geometry/intersection.h"

#include "integrityCheck.h"

#include "misc/validityHelper.h"

namespace {
	void recalculate(Part& part) {
		part.maxRadius = part.hitbox.getMaxRadius();
	}

	void recalculateAndUpdateParent(Part& part, const Bounds& oldBounds) {
		recalculate(part);
		if (part.parent != nullptr) {
			part.parent->updatePart(&part, oldBounds);
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
		parent->detachPart(this);
	}
}

PartIntersection Part::intersects(const Part& other) const {
	CFrame relativeTransform = this->cframe.globalToLocal(other.cframe);
	Intersection result = intersectsTransformed(this->hitbox, other.hitbox, relativeTransform);
	if(result.intersects) {
		Position intersection = this->cframe.localToGlobal(result.intersection);
		Vec3 exitVector = this->cframe.localToRelative(result.exitVector);


		CHECK_VALID_VEC(result.exitVector);


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
	Vec3 offset = this->cframe.getPosition() - parent->getObjectCenterOfMass();
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
	if(this->parent == nullptr) {
		MotorizedPhysical* newPhys = new MotorizedPhysical(this);
		this->parent = newPhys;
		this->parent->attachPart(other, constraint, attachToThis, attachToThat);
	} else {
		this->parent->attachPhysical(Physical(other, this->parent->mainPhysical), constraint, this->transformCFrameToParent(attachToThis), attachToThat);
	}
}

void Part::detach() {
	if(this->parent == nullptr) throw "No physical to detach from!";
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
		return this->parent->getAttachFor(this).attachment.localToGlobal(cframeRelativeToPart);
	}
}

bool Part::isMainPart() const {
	return this->parent == nullptr || this->parent->mainPart == this;
}

void Part::makeMainPart() {
	if(!this->isMainPart()) {
		this->parent->makeMainPart(this);
	}
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