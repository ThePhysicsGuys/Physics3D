#include "alignmentLink.h"

AlignmentLink::AlignmentLink(const AttachedPart& part1, const AttachedPart& part2) :
	SoftLink{ part1, part2 } {

	offset = this->attachedPart1.part->getCFrame().rotation.localToGlobal(this->attachedPart2.part->getCFrame().rotation);
}

void AlignmentLink::update() {
	
	Vec3 momentDir = getGlobalMoment();
	this->attachedPart1.part->applyMoment(momentDir);
	this->attachedPart2.part->applyMoment(momentDir);
}

Vec3 AlignmentLink::getGlobalMoment() noexcept {

	Rotation rot1 = this->attachedPart1.part->getCFrame().getRotation().localToGlobal(this->offset);
	Rotation rot2 = this->attachedPart2.part->getCFrame().getRotation();

	Rotation deltRot = rot2.globalToLocal(rot1);
	Vec3 momentDir = deltRot.asRotationVector();
	rot2.localToGlobal(momentDir);
	return momentDir;
	
}