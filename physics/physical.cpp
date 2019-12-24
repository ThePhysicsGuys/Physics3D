#include "physical.h"

#include "../util/log.h"

#include "inertia.h"
#include "world.h"
#include "math/linalg/mat.h"
#include "math/linalg/misc.h"
#include "math/linalg/trigonometry.h"

#include "debug.h"
#include <algorithm>
#include <limits>

#include "integrityCheck.h"

#include "misc/validityHelper.h"


/*
	===== Physical Structure =====
*/

#pragma region structure

Physical::Physical(Part* part, MotorizedPhysical* mainPhysical) : mainPart(part), mainPhysical(mainPhysical) {
	if (part->parent != nullptr) {
		throw "This part is already in another physical!";
	}
	part->parent = this;

	refreshWithNewParts();
}

Physical::~Physical() {
	if(mainPart != nullptr) {
		mainPart->parent = nullptr;
		mainPart = nullptr;
	}
	for(AttachedPart& atPart : parts) {
		atPart.part->parent = nullptr;
	}
}

Physical::Physical(Physical&& other) noexcept :
	mainPart(other.mainPart),
	parts(std::move(other.parts)),
	mainPhysical(other.mainPhysical),
	mass(other.mass),
	localCenterOfMass(other.localCenterOfMass),
	inertia(other.inertia) {
	mainPart->parent = this;
	other.mainPart = nullptr;
	for(AttachedPart& p : parts) {
		p.part->parent = this;
		p.part = nullptr;
	}
}

Physical& Physical::operator=(Physical&& other) noexcept {
	this->mainPart = other.mainPart;
	this->parts = std::move(other.parts);
	this->mainPhysical == other.mainPhysical;
	this->mass = other.mass;
	this->localCenterOfMass = other.localCenterOfMass;
	this->inertia = other.inertia;

	mainPart->parent = this;
	other.mainPart = nullptr;
	for(AttachedPart& p : other.parts) {
		p.part->parent = this;
		p.part = nullptr;
	}
	
	return *this;
}

void Physical::makeMainPart(Part* newMainPart) {
	if (newMainPart == mainPart) {
		Log::debug("Attempted to replace mainPart with mainPart");
		return;
	}
	for (AttachedPart& atPart : parts) {
		if (atPart.part == newMainPart) {
			makeMainPart(atPart);
			return;
		}
	}

	throw "Attempting to make a part not in this physical the mainPart!";
}

static bool liesInVector(const std::vector<AttachedPart>& vec, const AttachedPart* ptr) {
	return vec.begin()._Ptr <= ptr && vec.end()._Ptr < ptr;
}

void Physical::makeMainPart(AttachedPart& newMainPart) {
	if (liesInVector(parts, &newMainPart)) {
		CFrame& newCenterCFrame = newMainPart.attachment;
		std::swap(mainPart, newMainPart.part);
		for (AttachedPart& atPart : parts) {
			if (&atPart != &newMainPart) {
				atPart.attachment = newCenterCFrame.globalToLocal(atPart.attachment);
			}
		}
		newCenterCFrame = ~newCenterCFrame;
		parts.pop_back();
	} else {
		throw "Attempting to make a part not in this physical the mainPart!";
	}
}

void Physical::attachPhysical(Physical&& phys, const CFrame& attachment) {
	size_t originalAttachCount = this->parts.size();
	const GlobalCFrame& cf = this->getCFrame();
	phys.mainPart->cframe = cf.localToGlobal(attachment);
	this->parts.push_back(AttachedPart{attachment, phys.mainPart});
	phys.mainPart->parent = this;

	for (AttachedPart& ap : phys.parts) {
		CFrame globalAttach = attachment.localToGlobal(ap.attachment);
		this->parts.push_back(AttachedPart{globalAttach, ap.part});
		ap.part->cframe = cf.localToGlobal(globalAttach);
		ap.part->parent = this;
	}

	if (phys.mainPhysical->world != nullptr) {
		phys.mainPhysical->world->removeMainPhysical(phys.mainPhysical);
	}
	phys.mainPart = nullptr;
	phys.parts.clear();
	delete &phys;
	if (this->mainPhysical->world != nullptr) {
		NodeStack stack = this->mainPhysical->world->objectTree.findGroupFor(this->mainPart, this->mainPart->getStrictBounds());
		TreeNode& group = **stack;
		for (size_t i = originalAttachCount; i < this->parts.size(); i++) {
			Part* p = parts[i].part;
			this->mainPhysical->world->objectTree.addToExistingGroup(p, p->getStrictBounds(), group);
		}
		stack.expandBoundsAllTheWayToTop();
	}
}

bool Physical::isMainPhysical() const {
	Physical* ptr = mainPhysical;
	return ptr == this;
}

void Physical::removeChild(ConnectedPhysical* child) {
	assert(child >= this->childPhysicals.begin()._Ptr && child < this->childPhysicals.end()._Ptr);

	*child = std::move(childPhysicals.back());
	childPhysicals.pop_back();
}

void Physical::attachPhysical(Physical&& phys, HardConstraint* constraint, const CFrame& attachToThis, const CFrame& attachToThat) {
	if(phys.isMainPhysical()) {
		childPhysicals.push_back(ConnectedPhysical(std::move(phys), this, constraint, attachToThat, attachToThis));
		delete &phys;
	} else {
		ConnectedPhysical& connectedPhys = static_cast<ConnectedPhysical&>(phys);
		Physical* parent = connectedPhys.parent;
		assert(parent != nullptr);
		childPhysicals.push_back(ConnectedPhysical(std::move(phys), this, constraint, attachToThat, attachToThis));
		parent->removeChild(&connectedPhys);
	}

	

	childPhysicals.back().updateCFrame(this->getCFrame());

	mainPhysical->refreshPhysicalProperties();
}

void Physical::attachPart(Part* part, HardConstraint* constraint, const CFrame& attachToThis, const CFrame& attachToThat) {
	if(part->parent == nullptr) {
		childPhysicals.push_back(ConnectedPhysical(Physical(part, this->mainPhysical), this, constraint, attachToThat, attachToThis));
	} else {
		attachPhysical(std::move(*part->parent), constraint, attachToThis, fromRelativeToPartToRelativeToPhysical(part, attachToThat));
	}

	childPhysicals.back().updateCFrame(this->getCFrame());

	mainPhysical->refreshPhysicalProperties();
}


static Physical* findPhysicalParent(Physical* findIn, const ConnectedPhysical* toBeFound) {
	if(toBeFound >= findIn->childPhysicals.begin()._Ptr && toBeFound < findIn->childPhysicals.end()._Ptr) {
		return findIn;
	}
	for(ConnectedPhysical& p : findIn->childPhysicals) {
		Physical* result = findPhysicalParent(&p, toBeFound);
		if(result != nullptr) {
			return result;
		}
	}
	return nullptr;
}

void Physical::attachPart(Part* part, const CFrame& attachment) {
	if (part->parent != nullptr) { // part is already in a physical
		if (part->parent == this) { // move part within this physical
			if (part == this->mainPart) {
				for (AttachedPart& ap : parts) {
					ap.attachment = attachment.globalToLocal(ap.attachment);
				}
			} else {
				getAttachFor(part).attachment = attachment;
			}
			Bounds oldBounds = part->getStrictBounds();
			part->cframe = getCFrame().localToGlobal(attachment);
			if (this->mainPhysical->world != nullptr) {
				this->mainPhysical->world->updatePartBounds(part, oldBounds);
			}
		} else {
			// attach other part's entire physical
			if (part->parent->mainPart == part) {
				attachPhysical(std::move(*part->parent), attachment);
			} else {
				CFrame newAttach = attachment.localToGlobal(~part->parent->getAttachFor(part).attachment);
				attachPhysical(std::move(*part->parent), newAttach);
			}
		}
	} else {
		part->parent = this;
		parts.push_back(AttachedPart{ attachment, part });
		part->cframe = getCFrame().localToGlobal(attachment);

		if (this->mainPhysical->world != nullptr) {
			this->mainPhysical->world->objectTree.addToExistingGroup(part, part->getStrictBounds(), this->mainPart, this->mainPart->getStrictBounds());
		}
	}

	refreshWithNewParts();
}

// detaches part, without updating World
// Returns true if this physical has no more parts and should be deleted
void Physical::detachPart(Part* part) {
	for(auto iter = parts.begin(); iter != parts.end(); iter++) {
		AttachedPart& at = *iter;
		if(at.part == part) {
			part->parent = nullptr;
			parts.erase(iter);
			if(this->mainPhysical->world != nullptr) {
				this->mainPhysical->world->removePartFromTrees(part);
			}
			refreshWithNewParts();
			return;
		}
	}
	if (part == mainPart) {
		if (parts.size() == 0) {
			mainPart = nullptr;
		} else {
			makeMainPart(parts[parts.size() - 1]);
		}
		part->parent = nullptr;
		return;
	}
	throw "Error: could not find part to remove!";
}

void Physical::setCFrameUnsafe(const GlobalCFrame& newCFrame) {
	this->mainPart->cframe = newCFrame;
	for(const AttachedPart& p : parts) {
		p.part->cframe = newCFrame.localToGlobal(p.attachment);
	}
}

void Physical::updateAttachedPhysicals(double deltaT) {
	GlobalCFrame thisCFrame = getCFrame();
	for(ConnectedPhysical& p : childPhysicals) {
		p.constraintWithParent->update(deltaT);
		p.updateCFrame(thisCFrame);
	}
}

void Physical::setCFrame(const GlobalCFrame& newCFrame) {
	if(this->mainPhysical->world != nullptr) {
		Bounds oldMainPartBounds = this->mainPart->getStrictBounds();

		setCFrameUnsafe(newCFrame);

		this->mainPhysical->world->updatePartGroupBounds(this->mainPart, oldMainPartBounds);
	} else {
		setCFrameUnsafe(newCFrame);
	}
}

void Physical::setPartCFrame(Part* part, const GlobalCFrame& newCFrame) {
	if(part == mainPart) {
		setCFrame(newCFrame);
	} else {
		CFrame attach = getAttachFor(part).attachment;
		GlobalCFrame newMainCFrame = newCFrame.localToGlobal(~attach);

		setCFrame(newMainCFrame);
	}
}

#pragma endregion

/*
	===== Refresh functions =====
*/

#pragma region refresh

void Physical::refreshWithNewParts() {
	double totalMass = mainPart->getMass();
	Vec3 totalCenterOfMass = mainPart->getLocalCenterOfMass() * mainPart->getMass();
	for (const AttachedPart& p : parts) {
		totalMass += p.part->getMass();
		totalCenterOfMass += p.attachment.localToGlobal(p.part->getLocalCenterOfMass()) * p.part->getMass();
	}
	totalCenterOfMass /= totalMass;

	SymmetricMat3 totalInertia = getTranslatedInertiaAroundCenterOfMass(mainPart->getInertia(), mainPart->getLocalCenterOfMass(), totalCenterOfMass, mainPart->getMass());;

	for (const AttachedPart& p : parts) {
		const Part* part = p.part;
		totalInertia += getTransformedInertiaAroundCenterOfMass(part->getInertia(), part->getLocalCenterOfMass(), p.attachment, totalCenterOfMass, part->getMass());
	}
	this->mass = totalMass;
	this->localCenterOfMass = totalCenterOfMass;
	this->inertia = totalInertia;

	mainPhysical->refreshPhysicalProperties();
}

void Physical::updatePart(const Part* updatedPart, const Bounds& updatedBounds) {
	refreshWithNewParts();
	if (this->mainPhysical->world != nullptr) {
		this->mainPhysical->world->updatePartBounds(updatedPart, updatedBounds);
	}
}

void Physical::rotateAroundCenterOfMassUnsafe(const RotMat3& rotation) {
	Vec3 relCenterOfMass = getCFrame().localToRelative(localCenterOfMass);
	Vec3 relativeRotationOffset = rotation * relCenterOfMass - relCenterOfMass;
	mainPart->cframe.rotate(rotation);
	translateUnsafe(-relativeRotationOffset);
}
void Physical::translateUnsafe(const Vec3& translation) {
	mainPart->cframe.translate(translation);
}
void Physical::updateAttachedPartCFrames() {
	for (AttachedPart& attachment : parts) {
		attachment.part->cframe = getCFrame().localToGlobal(attachment.attachment);
	}
}
void Physical::rotateAroundCenterOfMass(const RotMat3& rotation) {
	Bounds oldBounds = this->mainPart->getStrictBounds();
	rotateAroundCenterOfMassUnsafe(rotation);
	updateAttachedPartCFrames();
	mainPhysical->world->updatePartGroupBounds(this->mainPart, oldBounds);
}
void Physical::translate(const Vec3& translation) {
	Bounds oldBounds = this->mainPart->getStrictBounds();
	translateUnsafe(translation);
	updateAttachedPartCFrames();
	mainPhysical->world->updatePartGroupBounds(this->mainPart, oldBounds);
}

static std::pair<Vec3, double> getRecursiveCenterOfMass(const Physical& phys) {
	Vec3 totalCOM = phys.mass * phys.localCenterOfMass;
	double totalMass = phys.mass;
	for(const ConnectedPhysical& conPhys : phys.childPhysicals) {
		CFrame relFrame = conPhys.getRelativeCFrameToParent();
		std::pair<Vec3, double> localCOM = getRecursiveCenterOfMass(conPhys);
		totalCOM += localCOM.second * relFrame.localToGlobal(localCOM.first);
		totalMass += localCOM.second;
	}
	return std::pair<Vec3, double>(totalCOM / totalMass, totalMass);
}

static SymmetricMat3 getRecursiveInertia(const Physical& phys, const CFrame& offsetCFrame, const Vec3& localCOMOfMain) {
	SymmetricMat3 totalInertia = getTransformedInertiaAroundCenterOfMass(phys.inertia, phys.localCenterOfMass, offsetCFrame, localCOMOfMain, phys.mass);

	for(const ConnectedPhysical& conPhys : phys.childPhysicals) {
		CFrame cframeToConPhys = conPhys.getRelativeCFrameToParent();

		CFrame offsetOfConPhys = offsetCFrame.localToGlobal(cframeToConPhys);

		totalInertia += getRecursiveInertia(conPhys, offsetOfConPhys, localCOMOfMain);
	}
	return totalInertia;
}

void MotorizedPhysical::refreshPhysicalProperties() {
	std::pair<Vec3, double> result = getRecursiveCenterOfMass(*this);
	totalCenterOfMass = result.first;
	totalMass = result.second;

	SymmetricMat3 totalInertia = getRecursiveInertia(*this, CFrame(0, 0, 0), totalCenterOfMass);

	forceResponse = SymmetricMat3::IDENTITY() * (1 / totalMass);
	momentResponse = ~totalInertia;
}

#pragma endregion

/*
	===== Update =====
*/

#pragma region update

void MotorizedPhysical::update(double deltaT) {
	refreshPhysicalProperties();

	Vec3 accel = forceResponse * totalForce * deltaT;
	
	Vec3 localMoment = getCFrame().relativeToLocal(totalMoment);
	Vec3 localRotAcc = momentResponse * localMoment * deltaT;
	Vec3 rotAcc = getCFrame().localToRelative(localRotAcc);

	totalForce = Vec3();
	totalMoment = Vec3();

	motionOfCenterOfMass.velocity += accel;
	motionOfCenterOfMass.angularVelocity += rotAcc;

	Vec3 movement = motionOfCenterOfMass.velocity * deltaT + accel * deltaT * deltaT / 2;

	Mat3 rotation = fromRotationVec(motionOfCenterOfMass.angularVelocity * deltaT);

	rotateAroundCenterOfMassUnsafe(rotation);
	translateUnsafe(movement);

	updateAttachedPartCFrames();

	updateAttachedPhysicals(deltaT);
}

void ConnectedPhysical::updateCFrame(const GlobalCFrame& parentCFrame) {
	GlobalCFrame newPosition = parentCFrame.localToGlobal(getRelativeCFrameToParent());
	setCFrameUnsafe(newPosition);
}

#pragma endregion

/*
	===== application of forces and the like
*/

#pragma region apply

void MotorizedPhysical::applyForceAtCenterOfMass(Vec3 force) {
	CHECK_VALID_VEC(force);
	totalForce += force;

	Debug::logVector(getCenterOfMass(), force, Debug::FORCE);
}

void MotorizedPhysical::applyForce(Vec3Relative origin, Vec3 force) {
	CHECK_VALID_VEC(origin);
	CHECK_VALID_VEC(force);
	totalForce += force;

	Debug::logVector(getCenterOfMass() + origin, force, Debug::FORCE);

	applyMoment(origin % force);
}

void MotorizedPhysical::applyMoment(Vec3 moment) {
	CHECK_VALID_VEC(moment);
	totalMoment += moment;
	Debug::logVector(getCenterOfMass(), moment, Debug::MOMENT);
}

void MotorizedPhysical::applyImpulseAtCenterOfMass(Vec3 impulse) {
	CHECK_VALID_VEC(impulse);
	Debug::logVector(getCenterOfMass(), impulse, Debug::IMPULSE);
	motionOfCenterOfMass.velocity += forceResponse * impulse;
}
void MotorizedPhysical::applyImpulse(Vec3Relative origin, Vec3Relative impulse) {
	CHECK_VALID_VEC(origin);
	CHECK_VALID_VEC(impulse);
	Debug::logVector(getCenterOfMass() + origin, impulse, Debug::IMPULSE);
	motionOfCenterOfMass.velocity += forceResponse * impulse;
	Vec3 angularImpulse = origin % impulse;
	applyAngularImpulse(angularImpulse);
}
void MotorizedPhysical::applyAngularImpulse(Vec3 angularImpulse) {
	CHECK_VALID_VEC(angularImpulse);
	Debug::logVector(getCenterOfMass(), angularImpulse, Debug::ANGULAR_IMPULSE);
	Vec3 localAngularImpulse = getCFrame().relativeToLocal(angularImpulse);
	Vec3 localRotAcc = momentResponse * localAngularImpulse;
	Vec3 rotAcc = getCFrame().localToRelative(localRotAcc);
	motionOfCenterOfMass.angularVelocity += rotAcc;
}

void MotorizedPhysical::applyDragAtCenterOfMass(Vec3 drag) {
	CHECK_VALID_VEC(drag);
	Debug::logVector(getCenterOfMass(), drag, Debug::POSITION);
	translate(forceResponse * drag);
}
void MotorizedPhysical::applyDrag(Vec3Relative origin, Vec3Relative drag) {
	CHECK_VALID_VEC(origin);
	CHECK_VALID_VEC(drag);
	Debug::logVector(getCenterOfMass() + origin, drag, Debug::POSITION);
	translateUnsafe(forceResponse * drag);
	Vec3 angularDrag = origin % drag;
	applyAngularDrag(angularDrag);
}
void MotorizedPhysical::applyAngularDrag(Vec3 angularDrag) {
	CHECK_VALID_VEC(angularDrag);
	Debug::logVector(getCenterOfMass(), angularDrag, Debug::INFO_VEC);
	Vec3 localAngularDrag = getCFrame().relativeToLocal(angularDrag);
	Vec3 localRotAcc = momentResponse * localAngularDrag;
	Vec3 rotAcc = getCFrame().localToRelative(localRotAcc);
	rotateAroundCenterOfMassUnsafe(fromRotationVec(rotAcc));
}


void Physical::applyDragToPhysical(Vec3 origin, Vec3 drag) {
	Vec3 COMOffset = mainPhysical->getCenterOfMass() - getCFrame().getPosition();
	mainPhysical->applyDrag(origin + COMOffset, drag);
}
void Physical::applyImpulseToPhysical(Vec3 origin, Vec3 impulse) {
	Vec3 COMOffset = mainPhysical->getCenterOfMass() - getCFrame().getPosition();
	mainPhysical->applyImpulse(origin + COMOffset, impulse);
}
void Physical::applyForceToPhysical(Vec3 origin, Vec3 force) {
	Vec3 COMOffset = mainPhysical->getCenterOfMass() - getCFrame().getPosition();
	mainPhysical->applyForce(origin + COMOffset, force);
}

#pragma endregion

#pragma region getters

Position Physical::getObjectCenterOfMass() const {
	return getCFrame().localToGlobal(localCenterOfMass);
}

GlobalCFrame Physical::getObjectCenterOfMassCFrame() const {
	return GlobalCFrame(getCFrame().localToGlobal(localCenterOfMass), getCFrame().getRotation());
}

double Physical::getVelocityKineticEnergy() const {
	return mass * lengthSquared(getMotion().velocity) / 2;
}
double Physical::getAngularKineticEnergy() const {
	Vec3 localAngularVel = getCFrame().relativeToLocal(getMotion().angularVelocity);
	return (inertia * localAngularVel) * localAngularVel / 2;
}
double Physical::getKineticEnergy() const {
	return getVelocityKineticEnergy() + getAngularKineticEnergy();
}
CFrame Physical::getRelativeCFrameToMain() const {
	return mainPhysical->getCFrame().globalToLocal(this->getCFrame());
}
Vec3 Physical::localToMain(const Vec3Local& vec) const {
	Position globalPos = this->getCFrame().localToGlobal(vec);
	return mainPhysical->getCFrame().globalToLocal(globalPos);
}

/*
	Computes the force->acceleration transformation matrix
	Such that:
	a = M*F
*/
SymmetricMat3 MotorizedPhysical::getResponseMatrix(const Vec3Local& r) const {
	Mat3 crossMat = createCrossProductEquivalent(r);

	SymmetricMat3 rotationFactor = multiplyLeftRight(momentResponse , crossMat);

	return forceResponse + rotationFactor;
}

Mat3 MotorizedPhysical::getResponseMatrix(const Vec3Local& actionPoint, const Vec3Local& responsePoint) const {
	Mat3 actionCross = createCrossProductEquivalent(actionPoint);
	Mat3 responseCross = createCrossProductEquivalent(responsePoint);

	Mat3 rotationFactor = responseCross * momentResponse * actionCross;

	return Mat3(forceResponse) - rotationFactor;
}
double MotorizedPhysical::getInertiaOfPointInDirectionLocal(const Vec3Local& localPoint, const Vec3Local& localDirection) const {
	SymmetricMat3 accMat = getResponseMatrix(localPoint);

	Vec3 force = localDirection;
	Vec3 accel = accMat * force;
	double accelInForceDir = accel * localDirection / lengthSquared(localDirection);

	return 1 / accelInForceDir;

	/*SymmetricMat3 accelToForceMat = ~accMat;
	Vec3 imaginaryForceForAcceleration = accelToForceMat * direction;
	double forcePerAccelRatio = imaginaryForceForAcceleration * direction / direction.lengthSquared();
	return forcePerAccelRatio;*/
}

double MotorizedPhysical::getInertiaOfPointInDirectionRelative(const Vec3Relative& relPoint, const Vec3Relative& relDirection) const {
	return getInertiaOfPointInDirectionLocal(getCFrame().relativeToLocal(relPoint), getCFrame().relativeToLocal(relDirection));
}

CFrame ConnectedPhysical::getRelativeCFrameToParent() const {
	return attachOnParent.localToGlobal(constraintWithParent->getRelativeCFrame().localToGlobal(~attachOnThis));
}

Position MotorizedPhysical::getCenterOfMass() const {
	return getCFrame().localToGlobal(totalCenterOfMass);
}

GlobalCFrame MotorizedPhysical::getCenterOfMassCFrame() const {
	return GlobalCFrame(getCFrame().localToGlobal(totalCenterOfMass), getCFrame().getRotation());
}

Motion Physical::getMotion() const {
	if(this->isMainPhysical()) {
		MotorizedPhysical* self = (MotorizedPhysical*) this;

		return self->motionOfCenterOfMass.getMotionOfPoint(getCFrame().localToRelative(self->localCenterOfMass - self->totalCenterOfMass));
	} else {
		ConnectedPhysical* self = (ConnectedPhysical*) this;
		
		Physical* parent = self->parent;

		// All motion and offset variables here are expressed in the global frame

		Motion parentMotion = parent->getMotion();

		Vec3 connectionOffsetOnParent = parent->getCFrame().localToRelative(self->attachOnParent.getPosition() - parent->localCenterOfMass);

		Motion motionOfConnectOnParent = parentMotion.getMotionOfPoint(connectionOffsetOnParent);

		Vec3 jointOffset = parent->getCFrame().localToRelative(self->attachOnParent.localToRelative(self->constraintWithParent->getRelativeCFrame().getPosition()));

		Motion motionPastJoint = motionOfConnectOnParent.addRelativeMotion(self->constraintWithParent->getRelativeMotion()).getMotionOfPoint(jointOffset);

		Vec3 connectionOffsetOnSelf = self->getCFrame().localToRelative(self->attachOnThis.getPosition() - self->localCenterOfMass);

		return motionPastJoint.getMotionOfPoint(-connectionOffsetOnSelf);
	}
}

#pragma endregion


ConnectedPhysical::ConnectedPhysical(Physical&& phys, Physical* parent, HardConstraint* constraintWithParent, const CFrame& attachOnThis, const CFrame& attachOnParent) :
	Physical(std::move(phys)), parent(parent), attachOnThis(attachOnThis), attachOnParent(attachOnParent), constraintWithParent(constraintWithParent) {
}

MotorizedPhysical::MotorizedPhysical(Part* mainPart) : Physical(mainPart, this) {
	refreshPhysicalProperties();
}

void MotorizedPhysical::ensureWorld(WorldPrototype* world) {
	if(this->world == world) return;
	if(this->world != nullptr) {
		this->world->removeMainPhysical(this);
	}
	throw "TODO";
}

#pragma region isValid

bool Physical::isValid() const {
	assert(isfinite(mass));
	assert(isVecValid(localCenterOfMass));
	assert(isMatValid(inertia));

	for(const AttachedPart& ap : parts) {
		assert(isCFrameValid(ap.attachment));
		assert(ap.part->isValid());
	}

	for(const ConnectedPhysical& p : childPhysicals) {
		assert(p.isValid());
	}

	return true;
}
bool MotorizedPhysical::isValid() const {
	assert(Physical::isValid());
	
	assert(isVecValid(totalForce));
	assert(isVecValid(totalMoment));
	assert(isfinite(totalMass));
	assert(isVecValid(totalCenterOfMass));
	assert(isMatValid(forceResponse));
	assert(isMatValid(momentResponse));

	return true;
}

bool ConnectedPhysical::isValid() const {
	assert(Physical::isValid());

	assert(isCFrameValid(attachOnParent));
	assert(isCFrameValid(attachOnThis));

	return true;
}

#pragma endregion

CFrame fromRelativeToPartToRelativeToPhysical(const Part* part, const CFrame& cframeRelativeToPart) {
	if(part->parent->mainPart == part) {
		return cframeRelativeToPart;
	} else {
		return part->parent->getAttachFor(part).attachment.localToGlobal(cframeRelativeToPart);
	}
}
