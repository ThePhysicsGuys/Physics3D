#include "elasticLink.h"

namespace P3D {
ElasticLink::ElasticLink(const AttachedPart& partA, const AttachedPart& partB, double restLength, double stiffness)
	: SoftLink(partA, partB)
	, restLength(restLength)
	, stiffness(stiffness) {}

void ElasticLink::update() {
	auto optionalForce = forceAppliedToTheLink();

	if (!optionalForce)
		return;

	Vec3 force = optionalForce.value();

	this->attachedPartA.part->applyForce(this->getRelativePositionOfAttachmentB(), force);
	this->attachedPartB.part->applyForce(this->getRelativePositionOfAttachmentA(), -force);
}

std::optional<Vec3> ElasticLink::forceAppliedToTheLink() {
	Vec3 difference = this->getGlobalPositionOfAttachmentA() - this->getGlobalPositionOfAttachmentB();
	double distance = length(difference);

	if (distance <= this->restLength)
		return std::nullopt;

	double magnitude = std::abs(distance - this->restLength) * this->stiffness;
	Vec3 forceDirection = normalize(difference);
	Vec3 force = magnitude * -forceDirection;

	return force;
}
};
