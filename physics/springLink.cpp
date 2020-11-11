

#include "springLink.h"


SpringLink::SpringLink(AttachedPart part1, AttachedPart part2, double restLength, double stiffness) :
	SoftLink{ part1, part2 },
	restLength{ restLength },
	stiffness{ stiffness } {

}

void SpringLink::update() {
	Vec3 force = forceAppliedToTheLink();
	this->attachedPart1.part->applyForce(this->getRelativePositionOfAttach1(), force);
	this->attachedPart2.part->applyForce(this->getRelativePositionOfAttach2(), -force);
}

Vec3 SpringLink::forceAppliedToTheLink() noexcept {
	Vec3 position = this->getLocalPositionOfAttach1() - this->getLocalPositionOfAttach2();
	double magnitude = length(position);
	double force = (magnitude - this->restLength) * this->stiffness;

	Vec3 direction = normalize(position);
	return direction * force;
}