

#include "springLink.h"
#include <cstdlib>


SpringLink::SpringLink(AttachedPart part1, AttachedPart part2, double restLength, double stiffness) :
	SoftLink{ part1, part2 },
	restLength{ restLength },
	stiffness{ stiffness } {

}

void SpringLink::update() {
	Vec3 force = forceAppliedToTheLink();
	this->attachedPart2.part->applyForce(this->getRelativePositionOfAttach1(), force);
	this->attachedPart1.part->applyForce(this->getRelativePositionOfAttach2(), -force);
}

Vec3 SpringLink::forceAppliedToTheLink() noexcept {
	Vec3 force = this->getGlobalPositionOfAttach2() - this->getGlobalPositionOfAttach1();
	double magnitude = length(force);
	magnitude = std::abs(magnitude - this->restLength);
	magnitude *= this->stiffness;
	force = normalize(force);
	force *= -magnitude;
	return force;
	
}