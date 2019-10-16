#include "part.h"

#include "physical.h"

#include "math/globalTransform.h"

#include "geometry/intersection.h"

namespace {
	void recalculate(Part& part) {
		part.maxRadius = part.hitbox.getMaxRadius();
		part.localBounds = part.hitbox.getBounds();
		part.mass = part.hitbox.getVolume() * part.properties.density;
		part.inertia = part.hitbox.getInertia() * part.properties.density;
		part.localCenterOfMass = part.hitbox.getCenterOfMass();
	}

	void recalculateAndUpdateParent(Part& part, const Bounds& oldBounds) {
		recalculate(part);
		if (part.parent != nullptr) {
			part.parent->updatePart(&part, oldBounds);
		}
	}
}

Part::Part(const Shape& shape, const GlobalCFrame& position, const PartProperties& properties) : hitbox(shape), cframe(position), properties(properties) {
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
		return PartIntersection(intersection, exitVector);
	}
	return PartIntersection();
}

Bounds Part::getStrictBounds() const {
	Fix<32> xmax = this->cframe.localToGlobal(this->hitbox.furthestInDirection(this->cframe.relativeToLocal(Vec3(1, 0, 0)))).x;
	Fix<32> xmin = this->cframe.localToGlobal(this->hitbox.furthestInDirection(this->cframe.relativeToLocal(Vec3(-1, 0, 0)))).x;
	Fix<32> ymax = this->cframe.localToGlobal(this->hitbox.furthestInDirection(this->cframe.relativeToLocal(Vec3(0, 1, 0)))).y;
	Fix<32> ymin = this->cframe.localToGlobal(this->hitbox.furthestInDirection(this->cframe.relativeToLocal(Vec3(0, -1, 0)))).y;
	Fix<32> zmax = this->cframe.localToGlobal(this->hitbox.furthestInDirection(this->cframe.relativeToLocal(Vec3(0, 0, 1)))).z;
	Fix<32> zmin = this->cframe.localToGlobal(this->hitbox.furthestInDirection(this->cframe.relativeToLocal(Vec3(0, 0, -1)))).z;

	return Bounds(Position(xmin, ymin, zmin), Position(xmax, ymax, zmax));
}

void Part::scale(double scaleX, double scaleY, double scaleZ) {
	Bounds oldBounds = this->getStrictBounds();
	this->hitbox = this->hitbox.scaled(scaleX, scaleY, scaleZ);
	recalculateAndUpdateParent(*this, oldBounds);
}

void Part::setCFrame(const GlobalCFrame& newCFrame) {
	if (this->parent == nullptr) {
		this->cframe = newCFrame;
	} else {
		this->parent->setPartCFrame(this, newCFrame);
	}
}


void Part::attach(Part& other, const CFrame& relativeCFrame) {
	if (this->parent == nullptr) {
		this->parent = new Physical(this);
		this->parent->attachPart(&other, relativeCFrame);
	} else {
		if (this->parent->mainPart == this) {
			this->parent->attachPart(&other, relativeCFrame);
		} else {
			CFrame trueCFrame = this->parent->getAttachFor(this).attachment.localToGlobal(relativeCFrame);
			this->parent->attachPart(&other, trueCFrame);
		}
	}
}

void Part::detach() {
	if (this->parent == nullptr) throw "No physical to detach from!";
	this->parent->detachPart(this);
}
