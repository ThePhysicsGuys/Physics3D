

#include "springLink.h"


SpringLink::SpringLink(AttachedPart part1, AttachedPart part2, double length, double preferredLength, double stiffness) :
	SoftLink{ part1, part2 },
	length{ length }, 
	preferredLength{ preferredLength },
	stiffness{ stiffness } {

}

Vec3 SpringLink::forceAppliedToTheLink() noexcept {
	double force = (this->length - this->preferredLength) * this->stiffness;
	Vec3 position = getPosition1() - getPosition2();
	Vec3 direction = normalize(position);
	return direction * force;
}

void SpringLink::update() {
	Vec3 force = forceAppliedToTheLink();
	Vec3 globalPosition = getGlobalLocationOfAttach1().getPosition() - getGlobalLocationOfAttach2().getPosition();
	this->attachedPart1.part->applyForce(globalPosition, force);
	this->attachedPart1.part->applyForce(globalPosition, -force);
}