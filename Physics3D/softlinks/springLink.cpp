#include "springLink.h"

namespace P3D {
SpringLink::SpringLink(const AttachedPart& partA, const AttachedPart& partB, double restLength, double stiffness)
	: SoftLink(partA, partB)
	, restLength(restLength)
	, stiffness(stiffness) {}

void SpringLink::update() {
	Vec3 force = forceAppliedToTheLink();

	this->attachedPartB.part->applyForce(this->getRelativePositionOfAttachmentA(), force);
	this->attachedPartA.part->applyForce(this->getRelativePositionOfAttachmentB(), -force);
}

Vec3 SpringLink::forceAppliedToTheLink() noexcept {
	Vec3 difference = this->getGlobalPositionOfAttachmentB() - this->getGlobalPositionOfAttachmentA();
	double distance = length(difference);

	double magnitude = std::abs(distance - this->restLength) * this->stiffness;

	Vec3 forceDirection = normalize(difference);
	return forceDirection * -magnitude;
}
};
