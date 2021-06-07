#include "rigidBody.h"

#include "inertia.h"

#include "misc/validityHelper.h"

#include <assert.h>

namespace P3D {
RigidBody::RigidBody(Part* mainPart) : 
	mainPart(mainPart), 
	mass(mainPart->getMass()),
	localCenterOfMass(mainPart->getLocalCenterOfMass()),
	inertia(mainPart->getInertia()) {}

void RigidBody::attach(RigidBody&& otherBody, const CFrame& attachment) {
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
void RigidBody::detach(Part* part) {
	for(AttachedPart& atPart : parts) {
		if(atPart.part == part) {
			parts.remove(std::move(atPart));
			refreshWithNewParts();
			return;
		}
	}

	throw "part not found!";
}
void RigidBody::detach(AttachedPart&& part) {
	parts.remove(std::move(part));
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

template<typename T>
static inline bool liesInVector(const std::vector<T>& vec, const T* ptr) {
	return &vec[0] <= ptr && &vec[0]+vec.size() > ptr;
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

void RigidBody::notifyPartStdMoved(Part* oldPartPtr, Part* newPartPtr) noexcept {
	if(this->mainPart == oldPartPtr) {
		this->mainPart = newPartPtr;
	} else {
		for(AttachedPart& atPart : this->parts) {
			if(atPart.part == oldPartPtr) {
				atPart.part = newPartPtr;
				return;
			}
		}
		assert(false); // cannot happen, part must be in this rigidbody
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

	SymmetricMat3 totalInertia = getTranslatedInertiaAroundCenterOfMass(mainPart->getInertia(), mainPart->getMass(), mainPart->getLocalCenterOfMass() - totalCenterOfMass);;

	for(const AttachedPart& p : parts) {
		const Part* part = p.part;
		CFrame attachRelativeToCOM = CFrame(p.attachment.getPosition() - totalCenterOfMass, p.attachment.getRotation());
		totalInertia += getTransformedInertiaAroundCenterOfMass(part->getInertia(), part->getMass(), part->getLocalCenterOfMass(), attachRelativeToCOM);
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

void RigidBody::setCFrameOfPart(Part* part, const GlobalCFrame& newCFrame) {
	if(part == mainPart) {
		setCFrame(newCFrame);
	} else {
		CFrame attach = getAttachFor(part).attachment;
		GlobalCFrame newMainCFrame = newCFrame.localToGlobal(~attach);

		setCFrame(newMainCFrame);
	}
}

void RigidBody::translate(const Vec3Fix& translation) {
	mainPart->cframe += translation;
	for(AttachedPart& atPart : parts) {
		atPart.part->cframe += translation;
	}
}

void RigidBody::rotateAroundLocalPoint(const Vec3& localPoint, const Rotation& rotation) {
	Vec3 relPoint = getCFrame().localToRelative(localPoint);
	Vec3 relativeRotationOffset = rotation * relPoint - relPoint;
	mainPart->cframe.rotate(rotation);
	mainPart->cframe -= relativeRotationOffset;
	for(AttachedPart& atPart : parts) {
		atPart.part->cframe = mainPart->cframe.localToGlobal(atPart.attachment);
	}
}

void RigidBody::setAttachFor(Part* part, const CFrame& attach) {
	if(mainPart == part) {
		for(AttachedPart& ap : parts) {
			ap.attachment = attach.globalToLocal(ap.attachment);
		}
	} else {
		AttachedPart& atPart = getAttachFor(part);
		atPart.attachment = attach;
	}
	refreshWithNewParts();
}

bool RigidBody::isValid() const {
	assert(std::isfinite(mass));
	assert(isVecValid(localCenterOfMass));
	assert(isMatValid(inertia));
	assert(mainPart->isValid());
	for(const AttachedPart& ap : parts) {
		assert(isCFrameValid(ap.attachment));
		assert(ap.part->isValid());
	}
	return true;
}
};