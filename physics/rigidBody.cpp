#include "rigidBody.h"

#include "inertia.h"

#include "../util/log.h"

RigidBody::RigidBody(Part* mainPart) : 
	mainPart(mainPart), 
	mass(mainPart->getMass()),
	localCenterOfMass(mainPart->getLocalCenterOfMass()),
	inertia(mainPart->getInertia()) {}

void RigidBody::attach(RigidBody&& otherBody, const CFrame& attachment) {
	size_t originalAttachCount = this->parts.size();
	const GlobalCFrame& cf = this->getCFrame();
	otherBody.mainPart->cframe = cf.localToGlobal(attachment);
	this->parts.push_back(AttachedPart{attachment, otherBody.mainPart});

	for(AttachedPart& ap : otherBody.parts) {
		CFrame globalAttach = attachment.localToGlobal(ap.attachment);
		this->parts.push_back(AttachedPart{globalAttach, ap.part});
		ap.part->cframe = cf.localToGlobal(globalAttach);
	}
}
void RigidBody::attach(Part* part, const CFrame& attachment) {
	parts.push_back(AttachedPart{attachment, part});
	part->cframe = getCFrame().localToGlobal(attachment);

	refreshWithNewParts();
}
void RigidBody::detach(AttachedPart& part) {
	part = std::move(parts.back());
	parts.pop_back();
	refreshWithNewParts();
	return;
}

const AttachedPart& RigidBody::getAttachFor(const Part* attachedPart) const {
	for(const AttachedPart& p : parts) {
		if(p.part == attachedPart) {
			return p;
		}
	}

	throw "Part not in this physical!";
}

AttachedPart& RigidBody::getAttachFor(const Part* attachedPart) {
	for(AttachedPart& p : parts) {
		if(p.part == attachedPart) {
			return p;
		}
	}

	throw "Part not in this physical!";
}

static bool liesInVector(const std::vector<AttachedPart>& vec, const AttachedPart* ptr) {
	return vec.begin()._Ptr <= ptr && vec.end()._Ptr > ptr;
}

// Not to be used by 
CFrame RigidBody::makeMainPart(AttachedPart& newMainPart) {
	if(liesInVector(parts, &newMainPart)) {
		// Update parts
		CFrame newCenterCFrame = newMainPart.attachment;
		std::swap(mainPart, newMainPart.part);
		for(AttachedPart& atPart : parts) {
			if(&atPart != &newMainPart) {
				atPart.attachment = newCenterCFrame.globalToLocal(atPart.attachment);
			}
		}
		newMainPart.attachment = ~newCenterCFrame; // since this CFrame is now the attachment of the former mainPart, we can just invert the original CFrame

		// refresh is needed for resyncing inertia, center of mass, etc
		refreshWithNewParts();

		return newCenterCFrame;
	} else {
		throw "Attempting to make a part not in this physical the mainPart!";
	}
}

void RigidBody::refreshWithNewParts() {
	double totalMass = mainPart->getMass();
	Vec3 totalCenterOfMass = mainPart->getLocalCenterOfMass() * mainPart->getMass();
	for(const AttachedPart& p : parts) {
		totalMass += p.part->getMass();
		totalCenterOfMass += p.attachment.localToGlobal(p.part->getLocalCenterOfMass()) * p.part->getMass();
	}
	totalCenterOfMass /= totalMass;

	SymmetricMat3 totalInertia = getTranslatedInertiaAroundCenterOfMass(mainPart->getInertia(), mainPart->getLocalCenterOfMass(), totalCenterOfMass, mainPart->getMass());;

	for(const AttachedPart& p : parts) {
		const Part* part = p.part;
		totalInertia += getTransformedInertiaAroundCenterOfMass(part->getInertia(), part->getLocalCenterOfMass(), p.attachment, totalCenterOfMass, part->getMass());
	}
	this->mass = totalMass;
	this->localCenterOfMass = totalCenterOfMass;
	this->inertia = totalInertia;
}

void RigidBody::setCFrame(const GlobalCFrame& newCFrame) {
	this->mainPart->cframe = newCFrame;
	for(const AttachedPart& p : parts) {
		p.part->cframe = newCFrame.localToGlobal(p.attachment);
	}
}

void RigidBody::setPartCFrame(Part* part, const GlobalCFrame& newCFrame) {
	if(part == mainPart) {
		setCFrame(newCFrame);
	} else {
		CFrame attach = getAttachFor(part).attachment;
		GlobalCFrame newMainCFrame = newCFrame.localToGlobal(~attach);

		setCFrame(newMainCFrame);
	}
}
