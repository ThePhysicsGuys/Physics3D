#include "magneticLink.h"


MagneticLink::MagneticLink(const AttachedPart& part1, const AttachedPart& part2,  const double magneticStrength):
	SoftLink{ part1, part2 },
	magneticStrength{ magneticStrength } {

}

void MagneticLink::update() {

	Vec3 force = forceAppliedToTheLink();

	this->attachedPart2.part->applyForce(this->getRelativePositionOfAttach1(), force);
	this->attachedPart1.part->applyForce(this->getRelativePositionOfAttach2(), -force);
}

Vec3 MagneticLink::forceAppliedToTheLink() noexcept {
	Vec3 forceVector = this->getGlobalPositionOfAttach1() - this->getGlobalPositionOfAttach2();

	Vec3 forceDirection = normalize(forceVector);
	double distanceBetweenParts = length(forceDirection);

	distanceBetweenParts *= distanceBetweenParts;

	return  this->magneticStrength * forceDirection / distanceBetweenParts;

}