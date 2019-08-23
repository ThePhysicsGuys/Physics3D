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
