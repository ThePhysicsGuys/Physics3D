#include "magneticLink.h"

namespace P3D {
MagneticLink::MagneticLink(const AttachedPart& partA, const AttachedPart& partB, double magneticStrength)
	: SoftLink(partA, partB)
	, magneticStrength(magneticStrength) {}

void MagneticLink::update() {
	Vec3 force = forceAppliedToTheLink();

	this->attachedPartB.part->applyForce(this->getRelativePositionOfAttachmentA(), force);
	this->attachedPartA.part->applyForce(this->getRelativePositionOfAttachmentB(), -force);
}

Vec3 MagneticLink::forceAppliedToTheLink() noexcept {
	Vec3 difference = this->getGlobalPositionOfAttachmentA() - this->getGlobalPositionOfAttachmentB();

	Vec3 forceDirection = normalize(difference);
	double distance = lengthSquared(difference);

	return this->magneticStrength * forceDirection / distance;

}
};
