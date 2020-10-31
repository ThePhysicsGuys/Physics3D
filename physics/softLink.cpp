#include "softLink.h"


SoftLink::AttachedPart::AttachedPart(CFrame attachment, Part* part) 
	: attachment{attachment},
	  part{part} { 

}

SoftLink::SoftLink(double stiffness, double length, double preferredLength, AttachedPart part1, AttachedPart part2) 
	:stiffness{ stiffness },  
	length{ length }, 
	preferredLength{ preferredLength }, 
	attachedPart1{part1}, 
	attachedPart2{part2} {

}

Vec3 SoftLink::forceAppliedToTheLink() {
	double force =  (this->length - this->preferredLength) * this->stiffness;
	Vec3 position = getPosition2() - getPosition1();
	Vec3 direction = normalize(position);
	return  direction * force;
}

void SoftLink::update() {
	Vec3 forceDirection = forceAppliedToTheLink();
	Vec3 relativeToOrigin = getGlobalLocationOfAttach1().getPosition() - getGlobalLocationOfAttach2().getPosition();
	this->attachedPart1.part->applyForce(relativeToOrigin, forceDirection);
	this->attachedPart2.part->applyForce(relativeToOrigin, forceDirection);
}

GlobalCFrame SoftLink::getCFrameOfPart1() const {
	return this->attachedPart1.part->getCFrame();
}

Position SoftLink::getPosition1() const {
	return getCFrameOfPart1().getPosition();
}

GlobalCFrame SoftLink::getCFrameOfPart2() const {
	return this->attachedPart2.part->getCFrame();
}

Position SoftLink::getPosition2() const {
	return getCFrameOfPart2().getPosition();
}

const GlobalCFrame& SoftLink::getGlobalLocationOfAttach1() const {
	return this->attachedPart1.part->getCFrame().localToGlobal(this->attachedPart1.attachment);
}

const GlobalCFrame& SoftLink::getGlobalLocationOfAttach2() const {
	return this->attachedPart2.part->getCFrame().localToGlobal(this->attachedPart2.attachment);
}


