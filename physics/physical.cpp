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

#include "misc/validityHelper.h"


/*
	===== Physical Structure =====
*/

#pragma region structure

Physical::Physical(Part* part, MotorizedPhysical* mainPhysical) : rigidBody(part), mainPhysical(mainPhysical) {
	if (part->parent != nullptr) {
		throw "This part is already in another physical!";
	}
	part->parent = this;
}

Physical::Physical(Physical&& other) noexcept :
	rigidBody(std::move(other.rigidBody)), 
	mainPhysical(other.mainPhysical),
	childPhysicals(std::move(other.childPhysicals)) {
	for(Part& p : this->rigidBody) {
		p.parent = this;
	}
	for(ConnectedPhysical& p : this->childPhysicals) {
		p.parent = this;
	}
}

Physical& Physical::operator=(Physical&& other) noexcept {
	this->rigidBody = std::move(other.rigidBody);
	this->mainPhysical = other.mainPhysical;
	this->childPhysicals = std::move(other.childPhysicals);
	for(Part& p : this->rigidBody) {
		p.parent = this;
	}
	for(ConnectedPhysical& p : this->childPhysicals) {
		p.parent = this;
	}
	
	return *this;
}

void Physical::makeMainPart(Part* newMainPart) {
	if (rigidBody.getMainPart() == newMainPart) {
		Log::warn("Attempted to replace mainPart with mainPart");
		return;
	}
	AttachedPart& atPart = rigidBody.getAttachFor(newMainPart);
	
	makeMainPart(atPart);
}

void Physical::makeMainPart(AttachedPart& newMainPart) {
	CFrame newCenterCFrame = rigidBody.makeMainPart(newMainPart);

	// Update attached physicals
	for(ConnectedPhysical& connectedPhys : childPhysicals) {
		connectedPhys.attachOnParent = newCenterCFrame.globalToLocal(connectedPhys.attachOnParent);
	}
	if(!isMainPhysical()) {
		ConnectedPhysical* self = (ConnectedPhysical*) this;
		self->attachOnThis = newCenterCFrame.globalToLocal(self->attachOnThis);
	}
}

static inline bool liesInVector(const std::vector<AttachedPart>& vec, const AttachedPart* ptr) {
	return vec.begin()._Ptr <= ptr && vec.end()._Ptr > ptr;
}

/*
	We will build a new tree, starting from a copy of the current physical

	S is the physical we are currently adding a child to
	OS is old location of S, which is now invalid and which will be replaced in the next iteration
	P (for parent) is the old parent of S, to be moved to be a child of S
	T (for trash) is the physical currently being replaced by P
	
	We wish to make S the new Main Physical

	      M
	     / \
	    /\
	   P
	  / \
	 S
	/ \

	Split S off from the tree
	Add a new empty node to S, call it T

	            .
	           / \
	 S        P
	/|\      / \
	   T   OS
	
	Copy P to T, reversing OS's attachment

	 S
	/|\        .
	   T      / \
	  / \    P
	OS

	T is the new S
	OS is the new T
	P is the new OS
	P's parent is the new P

	if P has no parent, then the separate tree of S is copied into it and is the new mainPhysical
*/
void ConnectedPhysical::makeMainPhysical() {
	Physical* P = this->parent;
	Physical newTop = std::move(*this);
	ConnectedPhysical* T = nullptr;
	ConnectedPhysical* OS = this;
	Physical* S = &newTop;

	// OS's Physical fields are invalid, but it's ConnectedPhysical fields are still valid

	bool firstRun = true;
	while(true) {
		bool PIsMain = P->isMainPhysical();
		// this part moves P down to be the child of S
		// swap attachOnParent and attachOnThis and use inverted constraint
		OS->constraintWithParent->invert();
		if(firstRun) {
			S->childPhysicals.push_back(ConnectedPhysical(std::move(*P),
									    S,
									    OS->constraintWithParent,
									    OS->attachOnParent,
									    OS->attachOnThis));
			T = &S->childPhysicals.back();
			firstRun = false;
		} else {
			*T = ConnectedPhysical(std::move(*P),
								   S,
								   OS->constraintWithParent,
								   OS->attachOnParent,
								   OS->attachOnThis);
		}
		
		OS->constraintWithParent = nullptr;

		// at this point, P::Physical is no longer valid, but the ConnectedPhysical's fields can still be used

		S = T;
		T = OS;

		if(!PIsMain) {
			ConnectedPhysical* OP = static_cast<ConnectedPhysical*>(P);
			OS = OP;
			P = OP->parent;
		} else {
			break;
		}
	}
	S->childPhysicals.remove(std::move(*T)); // remove the last trash Physical

	*P = std::move(newTop);

	MotorizedPhysical* OP = static_cast<MotorizedPhysical*>(P);
	OP->refreshPhysicalProperties();
}

bool Physical::isMainPhysical() const {
	Physical* ptr = mainPhysical;
	return ptr == this;
}

MotorizedPhysical* Physical::makeMainPhysical() {
	if(this->isMainPhysical()) {
		return static_cast<MotorizedPhysical*>(this);
	} else {
		MotorizedPhysical* main = this->mainPhysical;
		static_cast<ConnectedPhysical*>(this)->makeMainPhysical();
		return main;
	}
}

void Physical::removeChild(ConnectedPhysical* child) {
	assert(child >= this->childPhysicals.begin()._Ptr && child < this->childPhysicals.end()._Ptr);

	*child = std::move(childPhysicals.back());
	childPhysicals.pop_back();
}



void Physical::attachPhysical(Physical* phys, HardConstraint* constraint, const CFrame& attachToThis, const CFrame& attachToThat) {
	WorldPrototype* world = this->mainPhysical->world;
	if(world != nullptr) {
		world->mergePhysicals(this->mainPhysical, phys->mainPhysical);
	}

	if(phys->isMainPhysical()) {
		MotorizedPhysical* motorPhys = static_cast<MotorizedPhysical*>(phys);
		ConnectedPhysical childToAdd(std::move(*motorPhys), this, constraint, attachToThat, attachToThis);
		childPhysicals.push_back(std::move(childToAdd));
		ConnectedPhysical& p = childPhysicals.back();

		p.parent = this;
		p.setMainPhysicalRecursive(this->mainPhysical);

		delete motorPhys;
	} else {
		throw "todo makeMainPhysical";
		ConnectedPhysical* connectedPhys = static_cast<ConnectedPhysical*>(phys);
		Physical* parent = connectedPhys->parent;
		assert(parent != nullptr);
		ConnectedPhysical childToAdd(std::move(*phys), this, constraint, attachToThat, attachToThis);
		childPhysicals.push_back(std::move(childToAdd));
		parent->removeChild(connectedPhys);
	}




	childPhysicals.back().refreshCFrameRecursive();

	mainPhysical->refreshPhysicalProperties();
}

void Physical::attachPart(Part* part, HardConstraint* constraint, const CFrame& attachToThis, const CFrame& attachToThat) {
	if(part->parent == nullptr) {
		WorldPrototype* world = this->mainPhysical->world;
		if(world != nullptr) {
			world->mergePartAndPhysical(this->mainPhysical, part);
		}
		childPhysicals.push_back(ConnectedPhysical(Physical(part, this->mainPhysical), this, constraint, attachToThat, attachToThis));
		childPhysicals.back().refreshCFrame();
	} else {
		attachPhysical(part->parent, constraint, attachToThis, part->transformCFrameToParent(attachToThat));
	}

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

void Physical::attachPhysical(Physical* phys, const CFrame& attachment) {
	if(phys->isMainPhysical()) {
		attachPhysical(static_cast<MotorizedPhysical*>(phys), attachment);
	} else {
		throw "Not implemented!";
	}
}

void Physical::attachPhysical(MotorizedPhysical* phys, const CFrame& attachment) {
	this->childPhysicals.reserve(this->childPhysicals.size() + phys->childPhysicals.size());

	for(Part& p : phys->rigidBody) {
		p.parent = this;
	}
	this->rigidBody.attach(std::move(phys->rigidBody), attachment);
	

	for(ConnectedPhysical& conPhys : phys->childPhysicals) {
		this->childPhysicals.push_back(std::move(conPhys));
		ConnectedPhysical& conPhysOnThis = this->childPhysicals.back();
		conPhysOnThis.attachOnParent = attachment.localToGlobal(conPhysOnThis.attachOnParent);
		conPhysOnThis.parent = this;
		conPhysOnThis.setMainPhysicalRecursive(this->mainPhysical);
		conPhysOnThis.refreshCFrameRecursive();
	}

	WorldPrototype* world = this->mainPhysical->world;
	if(world != nullptr) {
		assert(phys->world == nullptr || phys->world == world);
		world->mergePhysicals(this->mainPhysical, phys->mainPhysical);
	}

	delete phys;

	mainPhysical->refreshPhysicalProperties();
}

void Physical::attachPart(Part* part, const CFrame& attachment) {
	if (part->parent != nullptr) { // part is already in a physical
		if (part->parent == this) {
			throw "Part already attached to this physical!";
		} else {
			// attach other part's entire physical
			if (part->isMainPart()) {
				attachPhysical(part->parent, attachment);
			} else {
				CFrame newAttach = attachment.localToGlobal(~part->parent->rigidBody.getAttachFor(part).attachment);
				attachPhysical(part->parent, newAttach);
			}
		}
	} else {
		if(this->mainPhysical->world != nullptr) {
			this->mainPhysical->world->mergePartAndPhysical(this->mainPhysical, part);
		}
		part->parent = this;
		rigidBody.attach(part, attachment);
	}
	this->mainPhysical->refreshPhysicalProperties();
}

void Physical::detachAllChildPhysicals() {
	WorldPrototype* world = this->mainPhysical->world;
	for(ConnectedPhysical& child : childPhysicals) {
		delete child.constraintWithParent;
		MotorizedPhysical* newPhys = new MotorizedPhysical(std::move(static_cast<Physical&>(child)));
		
		if(world != nullptr) {
			world->splitPhysical(this->mainPhysical, newPhys);
		}
	}

	childPhysicals.clear();
}


void Physical::detachChildAndGiveItNewMain(ConnectedPhysical&& formerChild) {
	delete formerChild.constraintWithParent;
	MotorizedPhysical* newPhys = new MotorizedPhysical(std::move(static_cast<Physical&>(formerChild)));
	WorldPrototype* world = this->mainPhysical->world;
	if(world != nullptr) {
		world->splitPhysical(this->mainPhysical, newPhys);
	}
	childPhysicals.remove(std::move(formerChild));
}

void Physical::detachAllHardConstraintsForSinglePartPhysical(bool alsoDelete) {
	this->detachAllChildPhysicals();
	MotorizedPhysical* mainPhys = this->mainPhysical; // save main physical because it'll get deleted by parent->detachChild()
	if(this != mainPhys) {
		ConnectedPhysical& self = static_cast<ConnectedPhysical&>(*this);
		Physical* parent = self.parent;
		if(alsoDelete) {
			parent->detachChildPartAndDelete(std::move(self));
		} else {
			parent->detachChildAndGiveItNewMain(std::move(self));
		}
		mainPhys->refreshPhysicalProperties();
	} else {
		if(alsoDelete) {
			if(mainPhys->world != nullptr) {
				mainPhys->world->removeMainPhysical(mainPhys);
			}
			delete mainPhys;
		}
	}
}

void Physical::detachChildPartAndDelete(ConnectedPhysical&& formerChild) {
	delete formerChild.constraintWithParent;
	WorldPrototype* world = this->mainPhysical->world;
	if(world != nullptr) {
		world->removePartFromTrees(formerChild.rigidBody.mainPart);
	}
	childPhysicals.remove(std::move(formerChild));
}

void Physical::detachFromRigidBody(Part* part) {
	part->parent = nullptr;
	rigidBody.detach(part);
}

void Physical::detachFromRigidBody(AttachedPart&& part) {
	part.part->parent = nullptr;
	rigidBody.detach(std::move(part));
}

void Physical::detachPart(Part* part, bool partStaysInWorld) {
	if(part == rigidBody.getMainPart()) {
		if(rigidBody.getPartCount() == 1) {
			// we have to disconnect this from other physicals as we're detaching the last part in the physical

			this->detachAllHardConstraintsForSinglePartPhysical(!partStaysInWorld);
			// new parent
			assert(part->parent->childPhysicals.size() == 0);
		} else {
			AttachedPart& newMainPartAndLaterOldPart = rigidBody.parts.back();
			makeMainPart(newMainPartAndLaterOldPart); // now points to old part
			detachFromRigidBody(std::move(newMainPartAndLaterOldPart));

			this->mainPhysical->refreshPhysicalProperties();
		}
	} else {
		detachFromRigidBody(part);

		this->mainPhysical->refreshPhysicalProperties();
	}

	if(partStaysInWorld) {
		new MotorizedPhysical(part);
	}
}

void Physical::notifyPartPropertiesChanged(Part* part) {
	rigidBody.refreshWithNewParts();
}
void Physical::notifyPartPropertiesAndBoundsChanged(Part* part, const Bounds& oldBounds) {
	notifyPartPropertiesChanged(part);
	if(this->mainPhysical->world != nullptr) {
		this->mainPhysical->world->updatePartBounds(part, oldBounds);
	}
}

void Physical::updateAttachedPhysicals(double deltaT) {
	for(ConnectedPhysical& p : childPhysicals) {
		p.constraintWithParent->update(deltaT);
		p.refreshCFrame();
		p.updateAttachedPhysicals(deltaT);
	}
}

void Physical::setCFrame(const GlobalCFrame& newCFrame) {
	if(this->mainPhysical->world != nullptr) {
		Bounds oldMainPartBounds = this->rigidBody.mainPart->getStrictBounds();

		rigidBody.setCFrame(newCFrame);

		this->mainPhysical->world->updatePartGroupBounds(this->rigidBody.mainPart, oldMainPartBounds);
	} else {
		rigidBody.setCFrame(newCFrame);
	}
}

void Physical::setPartCFrame(Part* part, const GlobalCFrame& newCFrame) {
	if(part == rigidBody.mainPart) {
		setCFrame(newCFrame);
	} else {
		CFrame attach = rigidBody.getAttachFor(part).attachment;
		GlobalCFrame newMainCFrame = newCFrame.localToGlobal(~attach);

		setCFrame(newMainCFrame);
	}
}

#pragma endregion

/*
	===== Refresh functions =====
*/

#pragma region refresh

void MotorizedPhysical::rotateAroundCenterOfMassUnsafe(const RotMat3& rotation) {
	rigidBody.rotateAroundLocalPoint(totalCenterOfMass, rotation);
}
void Physical::translateUnsafeRecursive(const Vec3Fix& translation) {
	rigidBody.translate(translation);
	for(ConnectedPhysical& conPhys : childPhysicals) {
		conPhys.translateUnsafeRecursive(translation);
	}
}
void MotorizedPhysical::rotateAroundCenterOfMass(const RotMat3& rotation) {
	Bounds oldBounds = this->rigidBody.mainPart->getStrictBounds();
	rotateAroundCenterOfMassUnsafe(rotation);
	mainPhysical->world->updatePartGroupBounds(this->rigidBody.mainPart, oldBounds);
}
void MotorizedPhysical::translate(const Vec3& translation) {
	Bounds oldBounds = this->rigidBody.mainPart->getStrictBounds();
	translateUnsafeRecursive(translation);
	mainPhysical->world->updatePartGroupBounds(this->rigidBody.mainPart, oldBounds);
}

static std::pair<Vec3, double> getRecursiveCenterOfMass(const Physical& phys) {
	Vec3 totalCOM = phys.rigidBody.mass * phys.rigidBody.localCenterOfMass;
	double totalMass = phys.rigidBody.mass;
	for(const ConnectedPhysical& conPhys : phys.childPhysicals) {
		CFrame relFrame = conPhys.getRelativeCFrameToParent();
		std::pair<Vec3, double> localCOM = getRecursiveCenterOfMass(conPhys);
		totalCOM += localCOM.second * relFrame.localToGlobal(localCOM.first);
		totalMass += localCOM.second;
	}
	return std::pair<Vec3, double>(totalCOM / totalMass, totalMass);
}

static SymmetricMat3 getRecursiveInertia(const Physical& phys, const CFrame& offsetCFrame, const Vec3& localCOMOfMain) {
	SymmetricMat3 totalInertia = getTransformedInertiaAroundCenterOfMass(phys.rigidBody.inertia, phys.rigidBody.localCenterOfMass, offsetCFrame, localCOMOfMain, phys.rigidBody.mass);

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

void ConnectedPhysical::refreshCFrame() {
	GlobalCFrame newPosition = parent->getCFrame().localToGlobal(getRelativeCFrameToParent());
	rigidBody.setCFrame(newPosition);
}

void ConnectedPhysical::refreshCFrameRecursive() {
	this->refreshCFrame();
	for(ConnectedPhysical& childPhys : childPhysicals) {
		childPhys.refreshCFrameRecursive();
	}
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
	translateUnsafeRecursive(movement);

	updateAttachedPhysicals(deltaT);
}

#pragma endregion

/*
	===== application of forces and the like
*/

#pragma region apply

void MotorizedPhysical::applyForceAtCenterOfMass(Vec3 force) {
	assert(isVecValid(force));
	totalForce += force;

	Debug::logVector(getCenterOfMass(), force, Debug::FORCE);
}

void MotorizedPhysical::applyForce(Vec3Relative origin, Vec3 force) {
	assert(isVecValid(origin));
	assert(isVecValid(force));
	totalForce += force;

	Debug::logVector(getCenterOfMass() + origin, force, Debug::FORCE);

	applyMoment(origin % force);
}

void MotorizedPhysical::applyMoment(Vec3 moment) {
	assert(isVecValid(moment));
	totalMoment += moment;
	Debug::logVector(getCenterOfMass(), moment, Debug::MOMENT);
}

void MotorizedPhysical::applyImpulseAtCenterOfMass(Vec3 impulse) {
	assert(isVecValid(impulse));
	Debug::logVector(getCenterOfMass(), impulse, Debug::IMPULSE);
	motionOfCenterOfMass.velocity += forceResponse * impulse;
}
void MotorizedPhysical::applyImpulse(Vec3Relative origin, Vec3Relative impulse) {
	assert(isVecValid(origin));
	assert(isVecValid(impulse));
	Debug::logVector(getCenterOfMass() + origin, impulse, Debug::IMPULSE);
	motionOfCenterOfMass.velocity += forceResponse * impulse;
	Vec3 angularImpulse = origin % impulse;
	applyAngularImpulse(angularImpulse);
}
void MotorizedPhysical::applyAngularImpulse(Vec3 angularImpulse) {
	assert(isVecValid(angularImpulse));
	Debug::logVector(getCenterOfMass(), angularImpulse, Debug::ANGULAR_IMPULSE);
	Vec3 localAngularImpulse = getCFrame().relativeToLocal(angularImpulse);
	Vec3 localRotAcc = momentResponse * localAngularImpulse;
	Vec3 rotAcc = getCFrame().localToRelative(localRotAcc);
	motionOfCenterOfMass.angularVelocity += rotAcc;
}

void MotorizedPhysical::applyDragAtCenterOfMass(Vec3 drag) {
	assert(isVecValid(drag));
	Debug::logVector(getCenterOfMass(), drag, Debug::POSITION);
	translate(forceResponse * drag);
}
void MotorizedPhysical::applyDrag(Vec3Relative origin, Vec3Relative drag) {
	assert(isVecValid(origin));
	assert(isVecValid(drag));
	Debug::logVector(getCenterOfMass() + origin, drag, Debug::POSITION);
	translateUnsafeRecursive(forceResponse * drag);
	Vec3 angularDrag = origin % drag;
	applyAngularDrag(angularDrag);
}
void MotorizedPhysical::applyAngularDrag(Vec3 angularDrag) {
	assert(isVecValid(angularDrag));
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

double Physical::getVelocityKineticEnergy() const {
	return rigidBody.mass * lengthSquared(getMotion().velocity) / 2;
}
double Physical::getAngularKineticEnergy() const {
	Vec3 localAngularVel = getCFrame().relativeToLocal(getMotion().angularVelocity);
	return (rigidBody.inertia * localAngularVel) * localAngularVel / 2;
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

		return self->motionOfCenterOfMass.getMotionOfPoint(getCFrame().localToRelative(rigidBody.localCenterOfMass - self->totalCenterOfMass));
	} else {
		ConnectedPhysical* self = (ConnectedPhysical*) this;
		
		Physical* parent = self->parent;

		// All motion and offset variables here are expressed in the global frame

		Motion parentMotion = parent->getMotion();

		Vec3 connectionOffsetOnParent = parent->getCFrame().localToRelative(self->attachOnParent.getPosition() - parent->rigidBody.localCenterOfMass);

		Motion motionOfConnectOnParent = parentMotion.getMotionOfPoint(connectionOffsetOnParent);

		Vec3 jointOffset = parent->getCFrame().localToRelative(self->attachOnParent.localToRelative(self->constraintWithParent->getRelativeCFrame().getPosition()));

		Motion motionPastJoint = motionOfConnectOnParent.addRelativeMotion(self->constraintWithParent->getRelativeMotion()).getMotionOfPoint(jointOffset);

		Vec3 connectionOffsetOnSelf = self->getCFrame().localToRelative(self->attachOnThis.getPosition() - rigidBody.localCenterOfMass);

		return motionPastJoint.getMotionOfPoint(-connectionOffsetOnSelf);
	}
}


size_t Physical::getNumberOfPartsInThisAndChildren() const {
	size_t totalParts = rigidBody.getPartCount();
	for(const ConnectedPhysical& child : childPhysicals) {
		totalParts += child.getNumberOfPartsInThisAndChildren();
	}
	return totalParts;
}

void Physical::setMainPhysicalRecursive(MotorizedPhysical* newMainPhysical) {
	this->mainPhysical = newMainPhysical;
	for(ConnectedPhysical& child : childPhysicals) {
		child.setMainPhysicalRecursive(newMainPhysical);
	}
}

#pragma endregion


ConnectedPhysical::ConnectedPhysical(Physical&& phys, Physical* parent, HardConstraint* constraintWithParent, const CFrame& attachOnThis, const CFrame& attachOnParent) :
	Physical(std::move(phys)), parent(parent), attachOnThis(attachOnThis), attachOnParent(attachOnParent), constraintWithParent(constraintWithParent) {
}

MotorizedPhysical::MotorizedPhysical(Part* mainPart) : Physical(mainPart, this) {
	refreshPhysicalProperties();
}

MotorizedPhysical::MotorizedPhysical(Physical&& movedPhys) : Physical(std::move(movedPhys)) {
	this->setMainPhysicalRecursive(this);
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
	assert(rigidBody.isValid());

	for(const ConnectedPhysical& p : childPhysicals) {
		assert(p.isValid());
		assert(p.parent == this);
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
