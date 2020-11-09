#include "softLink.h"


SoftLink::AttachedPart::AttachedPart(CFrame attachment, Part* part) 
	: attachment{attachment},
	  part{part} { 

}

SoftLink::SoftLink(AttachedPart part1, AttachedPart part2) :
	attachedPart1{part1}, 
	attachedPart2{part2} {
}

SoftLink::~SoftLink() {

}

GlobalCFrame SoftLink::getGlobalLocationOfAttach1() const {
	return this->attachedPart1.part->getCFrame().localToGlobal(this->attachedPart1.attachment);
}

GlobalCFrame SoftLink::getGlobalLocationOfAttach2() const {
	return this->attachedPart2.part->getCFrame().localToGlobal(this->attachedPart2.attachment);
}


