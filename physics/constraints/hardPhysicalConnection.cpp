#include "hardPhysicalConnection.h"

HardPhysicalConnection::HardPhysicalConnection(const CFrame& attachOnChild, const CFrame& attachOnParent, std::unique_ptr<HardConstraint> constraintWithParent) :
	attachOnChild(attachOnChild), 
	attachOnParent(attachOnParent), 
	constraintWithParent(std::move(constraintWithParent)) {}


CFrame HardPhysicalConnection::getRelativeCFrameToParent() const {
	return attachOnParent.localToGlobal(constraintWithParent->getRelativeCFrame().localToGlobal(~attachOnChild));
}

void HardPhysicalConnection::update(double deltaT) {
	constraintWithParent->update(deltaT);
}

HardPhysicalConnection HardPhysicalConnection::inverted() && {
	this->constraintWithParent->invert();
	
	return HardPhysicalConnection(this->attachOnParent, this->attachOnChild, std::move(this->constraintWithParent));
}


