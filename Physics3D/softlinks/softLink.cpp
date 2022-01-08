#include "softLink.h"

namespace P3D {

SoftLink::SoftLink(const AttachedPart& attachedPartA, const AttachedPart& attachedPartB)
	: attachedPartA(attachedPartA)
	, attachedPartB(attachedPartB) {}

SoftLink::~SoftLink() = default;

GlobalCFrame SoftLink::getGlobalCFrameOfAttachmentA() const {
	return this->attachedPartA.part->getCFrame();
}

GlobalCFrame SoftLink::getGlobalCFrameOfAttachmentB() const {
	return this->attachedPartB.part->getCFrame();
}

CFrame SoftLink::getLocalCFrameOfAttachmentA() const {
	return this->attachedPartA.attachment;
}

CFrame SoftLink::getLocalCFrameOfAttachmentB() const {
	return this->attachedPartB.attachment;
}

CFrame SoftLink::getRelativeOfAttachmentA() const {
	return this->getGlobalCFrameOfAttachmentA().localToRelative(this->attachedPartA.attachment);

}
CFrame SoftLink::getRelativeOfAttachmentB() const {
	return this->getGlobalCFrameOfAttachmentB().localToRelative(this->attachedPartB.attachment);
}

Position SoftLink::getGlobalPositionOfAttachmentB() const {
	return this->getGlobalCFrameOfAttachmentB().getPosition();
}
Position SoftLink::getGlobalPositionOfAttachmentA() const {
	return this->getGlobalCFrameOfAttachmentA().getPosition();
}
Vec3 SoftLink::getLocalPositionOfAttachmentA() const {
	return this->getLocalCFrameOfAttachmentA().getPosition();
}
Vec3 SoftLink::getLocalPositionOfAttachmentB() const {
	return this->getLocalCFrameOfAttachmentB().getPosition();
}
Vec3 SoftLink::getRelativePositionOfAttachmentA() const {
	return this->getRelativeOfAttachmentA().getPosition();
}
Vec3 SoftLink::getRelativePositionOfAttachmentB() const {
	return this->getRelativeOfAttachmentB().getPosition();
}

};
