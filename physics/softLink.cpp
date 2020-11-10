#include "softLink.h"


SoftLink::AttachedPart::AttachedPart(CFrame attachment, Part* part) : 
	attachment{attachment},
	part{part} { 

}

SoftLink::SoftLink(AttachedPart part1, AttachedPart part2) :
	attachedPart1{part1}, 
	attachedPart2{part2} {
}

SoftLink::~SoftLink() {
}

GlobalCFrame SoftLink::getGlobalCFrameOfAttach1() const {
	return this->attachedPart1.part->getCFrame();
}

GlobalCFrame SoftLink::getGlobalCFrameOfAttach2() const {
	return this->attachedPart2.part->getCFrame();
}

CFrame SoftLink::getLocalCFrameOfAttach1() const {
	return this->attachedPart1.attachment;
}

CFrame SoftLink::getLocalCFrameOfAttach2() const {
	return this->attachedPart2.attachment;
}

CFrame SoftLink::getRelativeOfAttach1() const {
	return this->getGlobalCFrameOfAttach1().localToRelative(this->attachedPart1.attachment);

}

CFrame SoftLink::getRelativeOfAttach2() const {
	return this->getGlobalCFrameOfAttach2().localToRelative(this->attachedPart2.attachment);
}

Position SoftLink::getGlobalPositionOfAttach2() const {
	return this->getGlobalCFrameOfAttach2().getPosition();
}

Position SoftLink::getGlobalPositionOfAttach1() const {
	return this->getGlobalCFrameOfAttach1().getPosition();
}

Vec3 SoftLink::getLocalPositionOfAttach1() const {
	return this->getLocalCFrameOfAttach1().getPosition();
}

Vec3 SoftLink::getLocalPositionOfAttach2() const {
	return this->getLocalCFrameOfAttach2().getPosition();
}

Vec3 SoftLink::getRelativePositionOfAttach1() const {
	return this->getRelativeOfAttach1().getPosition();
}

Vec3 SoftLink::getRelativePositionOfAttach2() const {
	return this->getRelativeOfAttach1().getPosition();
}





