#include "hardPhysicalConnection.h"

HardPhysicalConnection::HardPhysicalConnection(std::unique_ptr<HardConstraint> constraintWithParent, const CFrame& attachOnChild, const CFrame& attachOnParent) :
	attachOnChild(attachOnChild), 
	attachOnParent(attachOnParent), 
	constraintWithParent(std::move(constraintWithParent)) {}


CFrame HardPhysicalConnection::getRelativeCFrameToParent() const {
	return attachOnParent.localToGlobal(constraintWithParent->getRelativeCFrame().localToGlobal(~attachOnChild));
}

RelativeMotion HardPhysicalConnection::getRelativeMotion() const {
	RelativeMotion relMotion = constraintWithParent->getRelativeMotion();

	return relMotion.extendBegin(this->attachOnParent).extendEnd(~this->attachOnChild);
}

void HardPhysicalConnection::update(double deltaT) {
	constraintWithParent->update(deltaT);
}

HardPhysicalConnection HardPhysicalConnection::inverted() && {
	this->constraintWithParent->invert();
	
	return HardPhysicalConnection(std::move(this->constraintWithParent), this->attachOnParent, this->attachOnChild);
}
