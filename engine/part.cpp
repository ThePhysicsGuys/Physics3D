#include "part.h"

#include "physical.h"

namespace {
	void recalculate(Part& part) {
		part.maxRadius = part.hitbox.getMaxRadius();
		part.localBounds = part.hitbox.getBounds();
		part.mass = part.hitbox.getVolume() * part.properties.density;
		part.inertia = part.hitbox.getInertia() * part.properties.density;
		part.localCenterOfMass = part.hitbox.getCenterOfMass();
	}

	void recalculateAndUpdateParent(Part& part) {
		recalculate(part);
		if (part.parent != nullptr) {
			part.parent->refreshWithNewParts();
		}
	}
}

Part::Part(const Shape& shape, const GlobalCFrame& position, double density, double friction) : hitbox(shape), cframe(position), properties({density, friction}) {
	recalculate(*this);
}

Part::~Part() {
	if (parent != nullptr) {
		parent->detachPart(this);
	}
}

bool Part::intersects(const Part& other, Position& intersection, Vec3& exitVector) const {
	const CFramef relativeCFrame(this->cframe.globalToLocal(other.cframe));
	Vec3f localIntersection, localExitVector;
	if(this->hitbox.intersectsTransformed(other.hitbox, relativeCFrame, localIntersection, localExitVector)) {
		intersection = this->cframe.localToGlobal(Vec3(localIntersection));
		exitVector = this->cframe.localToRelative(Vec3(localExitVector));
		return true;
	}
	return false;
}

void Part::scale(double scaleX, double scaleY, double scaleZ) {
	this->hitbox = this->hitbox.scaled(scaleX, scaleY, scaleZ);
	recalculateAndUpdateParent(*this);
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
