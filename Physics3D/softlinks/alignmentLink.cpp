#include "alignmentLink.h"

namespace P3D {
AlignmentLink::AlignmentLink(const AttachedPart& partA, const AttachedPart& partB) :
	SoftLink(partA, partB) {

	offset = this->attachedPartA.part->getCFrame().rotation.localToGlobal(this->attachedPartB.part->getCFrame().rotation);
}

void AlignmentLink::update() {
	Vec3 momentDir = getGlobalMoment();
	this->attachedPartA.part->applyMoment(momentDir);
	this->attachedPartB.part->applyMoment(momentDir);
}

Vec3 AlignmentLink::getGlobalMoment() noexcept {
	Rotation rot1 = this->attachedPartA.part->getCFrame().getRotation().localToGlobal(this->offset);
	Rotation rot2 = this->attachedPartB.part->getCFrame().getRotation();

	Rotation deltRot = rot2.globalToLocal(rot1);
	Vec3 momentDir = deltRot.asRotationVector();

	return momentDir;	
}
};