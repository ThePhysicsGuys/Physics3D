#include "physical.h"

#include "inertia.h"
#include "world.h"
#include "math/linalg/mat.h"
#include "math/linalg/trigonometry.h"

#include "misc/debug.h"
#include "misc/validityHelper.h"
#include "misc/unreachable.h"

#include "layer.h"

#include <algorithm>
#include <cassert>


/*
	===== Physical Structure =====
*/

namespace P3D {
#pragma region structure

Physical::Physical(Part* part, MotorizedPhysical* mainPhysical) : rigidBody(part), mainPhysical(mainPhysical) {
	if (part->parent != nullptr) {
		throw "This part is already in another physical!";
	}
	part->parent = this;
}

Physical::Physical(RigidBody&& rigidBody, MotorizedPhysical* mainPhysical) : rigidBody(std::move(rigidBody)), mainPhysical(mainPhysical) {
	for(Part& p : this->rigidBody) {
		if(p.parent != nullptr) {
			throw "This part is already in another physical!";
		}
		p.parent = this;
	}
}

Physical::Physical(Physical&& other) noexcept :
	rigidBody(std::move(other.rigidBody)), 
	mainPhysical(other.mainPhysical),
	childPhysicals(std::move(other.childPhysicals)) {
	this->rigidBody.mainPart->parent = this;
	for(AttachedPart& p : this->rigidBody.parts) {
		p.part->parent = this;
	}
	for(ConnectedPhysical& p : this->childPhysicals) {
		p.parent = this;
	}
}

Physical& Physical::operator=(Physical&& other) noexcept {
	this->rigidBody = std::move(other.rigidBody);
	this->mainPhysical = other.mainPhysical;
	this->childPhysicals = std::move(other.childPhysicals);
	this->rigidBody.mainPart->parent = this;
	for(AttachedPart& p : this->rigidBody.parts) {
		p.part->parent = this;
	}
	for(ConnectedPhysical& p : this->childPhysicals) {
		p.parent = this;
	}
	
	return *this;
}

ConnectedPhysical::ConnectedPhysical(RigidBody&& rigidBody, Physical* parent, HardPhysicalConnection&& connectionToParent) :
	Physical(std::move(rigidBody), parent->mainPhysical), parent(parent), connectionToParent(std::move(connectionToParent)) {}

ConnectedPhysical::ConnectedPhysical(Physical&& phys, Physical* parent, HardPhysicalConnection&& connectionToParent) :
	Physical(std::move(phys)), parent(parent), connectionToParent(std::move(connectionToParent)) {}

ConnectedPhysical::ConnectedPhysical(Physical&& phys, Physical* parent, HardConstraint* constraintWithParent, const CFrame& attachOnThis, const CFrame& attachOnParent) :
	Physical(std::move(phys)), parent(parent), connectionToParent(std::unique_ptr<HardConstraint>(constraintWithParent), attachOnThis, attachOnParent) {}

MotorizedPhysical::MotorizedPhysical(Part* mainPart) : Physical(mainPart, this) {
	refreshPhysicalProperties();
}

MotorizedPhysical::MotorizedPhysical(RigidBody&& rigidBody) : Physical(std::move(rigidBody), this) {
	refreshPhysicalProperties();
}

MotorizedPhysical::MotorizedPhysical(Physical&& movedPhys) : Physical(std::move(movedPhys)) {
	this->setMainPhysicalRecursive(this);
	refreshPhysicalProperties();
}

void Physical::makeMainPart(Part* newMainPart) {
	if (rigidBody.getMainPart() == newMainPart) {
		Debug::logWarn("Attempted to replace mainPart with mainPart");
		return;
	}
	AttachedPart& atPart = rigidBody.getAttachFor(newMainPart);
	
	makeMainPart(atPart);
}

void Physical::makeMainPart(AttachedPart& newMainPart) {
	CFrame newCenterCFrame = rigidBody.makeMainPart(newMainPart);

	// Update attached physicals
	for(ConnectedPhysical& connectedPhys : childPhysicals) {
		connectedPhys.connectionToParent.attachOnParent = newCenterCFrame.globalToLocal(connectedPhys.connectionToParent.attachOnParent);
	}
	if(!isMainPhysical()) {
		ConnectedPhysical* self = (ConnectedPhysical*) this;
		self->connectionToParent.attachOnChild = newCenterCFrame.globalToLocal(self->connectionToParent.attachOnChild);
	}
}

template<typename T>
static inline bool liesInVector(const std::vector<T>& vec, const T* ptr) {
	return &vec[0] <= ptr && &vec[0]+vec.size() > ptr;
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
		if(firstRun) {
			S->childPhysicals.push_back(ConnectedPhysical(std::move(*P),
									    S,
									    std::move(*OS).connectionToParent.inverted()));
			T = &S->childPhysicals.back();
			firstRun = false;
		} else {
			*T = ConnectedPhysical(std::move(*P),
								   S,
								   std::move(*OS).connectionToParent.inverted());
		}

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

std::size_t ConnectedPhysical::getIndexInParent() const {
	return &this->parent->childPhysicals[0] - this;
}

RelativeMotion ConnectedPhysical::getRelativeMotionBetweenParentAndSelf() const {
	return connectionToParent.getRelativeMotion();
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
	assert(liesInVector(this->childPhysicals, child));

	*child = std::move(childPhysicals.back());
	childPhysicals.pop_back();
}



void Physical::attachPhysical(Physical* phys, HardConstraint* constraint, const CFrame& attachToThis, const CFrame& attachToThat) {
	this->attachPhysical(phys->makeMainPhysical(), constraint, attachToThis, attachToThat);
}

static void removePhysicalFromList(std::vector<MotorizedPhysical*>& physicals, MotorizedPhysical* physToRemove) {
	for(MotorizedPhysical*& item : physicals) {
		if(item == physToRemove) {
			item = std::move(physicals.back());
			physicals.pop_back();
			return;
		}
	}
	unreachable(); // No physical found to remove!
}

void Physical::attachPhysical(MotorizedPhysical* phys, HardConstraint* constraint, const CFrame& attachToThis, const CFrame& attachToThat) {
	WorldPrototype* world = this->getWorld();
	if(world != nullptr) {
		if(mainPhysical->getWorld() != nullptr) {
			assert(mainPhysical->getWorld() == world);
			removePhysicalFromList(world->physicals, mainPhysical);
		}
	}

	ConnectedPhysical childToAdd(std::move(*phys), this, constraint, attachToThat, attachToThis);
	childPhysicals.push_back(std::move(childToAdd));
	ConnectedPhysical& p = childPhysicals.back();

	p.parent = this;
	p.setMainPhysicalRecursive(this->mainPhysical);

	delete phys;

	childPhysicals.back().refreshCFrameRecursive();

	mainPhysical->refreshPhysicalProperties();
}

void Physical::attachPart(Part* part, HardConstraint* constraint, const CFrame& attachToThis, const CFrame& attachToThat) {
	if(part->parent == nullptr) {
		WorldPrototype* world = this->getWorld();
		if(world != nullptr) {
			world->onPartAdded(part);
		}
		childPhysicals.push_back(ConnectedPhysical(Physical(part, this->mainPhysical), this, constraint, attachToThat, attachToThis));
		childPhysicals.back().refreshCFrame();
	} else {
		attachPhysical(part->parent, constraint, attachToThis, part->transformCFrameToParent(attachToThat));
	}

	mainPhysical->refreshPhysicalProperties();
}


static Physical* findPhysicalParent(Physical* findIn, const ConnectedPhysical* toBeFound) {
	if(liesInVector(findIn->childPhysicals, toBeFound)) {
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
	attachPhysical(phys->makeMainPhysical(), attachment);
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
		conPhysOnThis.connectionToParent.attachOnParent = attachment.localToGlobal(conPhysOnThis.connectionToParent.attachOnParent);
		conPhysOnThis.parent = this;
		conPhysOnThis.setMainPhysicalRecursive(this->mainPhysical);
		conPhysOnThis.refreshCFrameRecursive();
	}

	WorldPrototype* world = this->getWorld();
	if(world != nullptr) {
		if(phys->getWorld() != nullptr) {
			assert(phys->getWorld() == world);
			removePhysicalFromList(world->physicals, phys->mainPhysical);
		}
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
		WorldPrototype* world = this->getWorld();
		if(world != nullptr) {
			world->onPartAdded(part);
		}
		part->parent = this;
		rigidBody.attach(part, attachment);
	}
	this->mainPhysical->refreshPhysicalProperties();
}

void Physical::detachAllChildPhysicals() {
	WorldPrototype* world = this->getWorld();
	for(ConnectedPhysical& child : childPhysicals) {
		MotorizedPhysical* newPhys = new MotorizedPhysical(std::move(static_cast<Physical&>(child)));
		
		if(world != nullptr) {
			world->notifyPhysicalHasBeenSplit(this->mainPhysical, newPhys);
		}
	}

	childPhysicals.clear(); // calls the destructors on all (now invalid) children, deleting the constraints in the process
}

void Physical::detachFromRigidBody(Part* part) {
	part->parent = nullptr;
	rigidBody.detach(part);
	WorldPrototype* world = this->getWorld();
	// TODO?
}

void Physical::detachFromRigidBody(AttachedPart&& part) {
	part.part->parent = nullptr;
	rigidBody.detach(std::move(part));
}

static void computeInternalRelativeMotionTree(MonotonicTreeBuilder<RelativeMotion>& builder, MonotonicTreeNode<RelativeMotion>& curNode, const ConnectedPhysical& conPhys, const RelativeMotion& motionOfParent) {
	RelativeMotion motionOfSelf = motionOfParent + conPhys.getRelativeMotionBetweenParentAndSelf();

	curNode.value = motionOfSelf.extendEnd(conPhys.rigidBody.localCenterOfMass);

	std::size_t size = conPhys.childPhysicals.size();
	
	if(size == 0) {
		curNode.children = nullptr;
	} else { 
		curNode.children = builder.alloc(size);
		for(std::size_t i = 0; i < size; i++) {
			computeInternalRelativeMotionTree(builder, curNode.children[i], conPhys.childPhysicals[i], motionOfSelf);
		}
	}
}

InternalMotionTree MotorizedPhysical::getInternalRelativeMotionTree(UnmanagedArray<MonotonicTreeNode<RelativeMotion>>&& mem) const noexcept {
	MonotonicTreeBuilder<RelativeMotion> builder(std::move(mem));

	std::size_t mainSize = this->childPhysicals.size();

	MonotonicTreeNode<RelativeMotion>* childNodes = builder.alloc(mainSize);
	for(std::size_t i = 0; i < mainSize; i++) {
		MonotonicTreeNode<RelativeMotion>& curNode = childNodes[i];
		const ConnectedPhysical& conPhys = this->childPhysicals[i];
		RelativeMotion motionOfPrimallyAttached = conPhys.getRelativeMotionBetweenParentAndSelf();

		curNode.value = motionOfPrimallyAttached.extendEnd(conPhys.rigidBody.localCenterOfMass);

		std::size_t size = conPhys.childPhysicals.size();

		if(size == 0) {
			curNode.children = nullptr;
		} else {
			curNode.children = builder.alloc(size);
			for(std::size_t j = 0; j < size; j++) {
				computeInternalRelativeMotionTree(builder, curNode.children[j], conPhys.childPhysicals[j], motionOfPrimallyAttached);
			}
		}
	}

	return InternalMotionTree(this, MonotonicTree<RelativeMotion>(std::move(builder)));
}

COMMotionTree MotorizedPhysical::getCOMMotionTree(UnmanagedArray<MonotonicTreeNode<RelativeMotion>>&& mem) const noexcept {
	return this->getInternalRelativeMotionTree(std::move(mem)).normalizeCenterOfMass();
}

void Physical::detachPartAssumingMultipleParts(Part* part) {
	assert(part->parent == this);
	assert(rigidBody.getPartCount() > 1);

	if(part == rigidBody.getMainPart()) {
		AttachedPart& newMainPartAndLaterOldPart = rigidBody.parts.back();
		makeMainPart(newMainPartAndLaterOldPart); // now points to old part
		detachFromRigidBody(std::move(newMainPartAndLaterOldPart));
	} else {
		detachFromRigidBody(part);
	}

	this->mainPhysical->refreshPhysicalProperties();
}

void Physical::detachPart(Part* part) {
	assert(part->parent == this);

	WorldPrototype* world = this->getWorld();

	if(rigidBody.getPartCount() == 1) {
		assert(part == rigidBody.getMainPart());

		// we have to disconnect this from other physicals as we're detaching the last part in the physical

		this->detachAllChildPhysicals();
		MotorizedPhysical* mainPhys = this->mainPhysical; // save main physical because it'll get deleted by parent->detachChild()
		if(this != mainPhys) {
			ConnectedPhysical& self = static_cast<ConnectedPhysical&>(*this);
			MotorizedPhysical* newPhys = new MotorizedPhysical(std::move(static_cast<Physical&>(self)));
			if(world != nullptr) {
				world->notifyPhysicalHasBeenSplit(mainPhys, newPhys);
			}
			self.parent->childPhysicals.remove(std::move(self)); // double move, but okay, since remove really only needs the address of self
			mainPhys->refreshPhysicalProperties();
		}

		// After this, self, and hence also *this* is no longer valid!
		// It has been removed
	} else {
		detachPartAssumingMultipleParts(part);
		MotorizedPhysical* newPhys = new MotorizedPhysical(part);
		part->parent = newPhys;
		if(world != nullptr) {
			world->physicals.push_back(newPhys);
		}
	}
}

void Physical::removePart(Part* part) {
	assert(part->parent == this);

	if(rigidBody.getPartCount() == 1) {
		assert(part == rigidBody.getMainPart());

		// we have to disconnect this from other physicals as we're removing the last part in the physical

		this->detachAllChildPhysicals();
		MotorizedPhysical* mainPhys = this->mainPhysical; // save main physical because it'll get deleted by parent->detachChild()
		if(this != mainPhys) {
			ConnectedPhysical& self = static_cast<ConnectedPhysical&>(*this);
			self.parent->childPhysicals.remove(std::move(self));
			mainPhys->refreshPhysicalProperties();
		} else {
			WorldPrototype* mainPhysWorld = this->getWorld();
			if(mainPhysWorld != nullptr) {
				removePhysicalFromList(mainPhysWorld->physicals, mainPhys);
			}
			delete mainPhys;
		}

		// After this, self, and hence also *this* is no longer valid!
		// It has been removed
	} else {
		detachPartAssumingMultipleParts(part);
	}
	part->parent = nullptr;
}

// TODO: this seems to need to update the encompassing MotorizedPhysical as well
void Physical::notifyPartPropertiesChanged(Part* part) {
	rigidBody.refreshWithNewParts();
}
void Physical::notifyPartStdMoved(Part* oldPartPtr, Part* newPartPtr) noexcept {
	rigidBody.notifyPartStdMoved(oldPartPtr, newPartPtr);
}

void Physical::updateConstraints(double deltaT) {
	for(ConnectedPhysical& p : childPhysicals) {
		p.connectionToParent.update(deltaT);
		p.updateConstraints(deltaT);
	}
}

void Physical::updateAttachedPhysicals() {
	for(ConnectedPhysical& p : childPhysicals) {
		p.refreshCFrame();
		p.updateAttachedPhysicals();
	}
}

void Physical::setCFrame(const GlobalCFrame& newCFrame) {
	if(this->isMainPhysical()) {
		MotorizedPhysical* motorThis = static_cast<MotorizedPhysical*>(this);
		motorThis->setCFrame(newCFrame);
	} else {
		ConnectedPhysical* connectedThis = static_cast<ConnectedPhysical*>(this);
		connectedThis->setCFrame(newCFrame);
	}
}

void ConnectedPhysical::setCFrame(const GlobalCFrame& newCFrame) {
	CFrame relativeCFrameToParent = this->getRelativeCFrameToParent();

	GlobalCFrame resultingCFrame = newCFrame.localToGlobal(~relativeCFrameToParent);

	this->parent->setCFrame(resultingCFrame);
}

void MotorizedPhysical::setCFrame(const GlobalCFrame& newCFrame) {
	rigidBody.setCFrame(newCFrame);
	for(ConnectedPhysical& conPhys : childPhysicals) {
		conPhys.refreshCFrameRecursive();
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

void MotorizedPhysical::rotateAroundCenterOfMass(const Rotation& rotation) {
	rigidBody.rotateAroundLocalPoint(totalCenterOfMass, rotation);
}
void Physical::translateUnsafeRecursive(const Vec3Fix& translation) {
	rigidBody.translate(translation);
	for(ConnectedPhysical& conPhys : childPhysicals) {
		conPhys.translateUnsafeRecursive(translation);
	}
}
void MotorizedPhysical::translate(const Vec3& translation) {
	translateUnsafeRecursive(translation);
}

void MotorizedPhysical::refreshPhysicalProperties() {
	ALLOCA_COMMotionTree(cache, this, size);

	this->totalCenterOfMass = cache.centerOfMass;
	this->totalMass = cache.totalMass;

	SymmetricMat3 totalInertia = cache.getInertia();

	this->forceResponse = SymmetricMat3::IDENTITY() * (1 / cache.totalMass);
	this->momentResponse = ~totalInertia;
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

void MotorizedPhysical::fullRefreshOfConnectedPhysicals() {
	for(ConnectedPhysical& conPhys : childPhysicals) {
		conPhys.refreshCFrameRecursive();
	}
}

#pragma endregion

/*
	===== Update =====
*/

#pragma region update

void MotorizedPhysical::update(double deltaT) {

	Vec3 accel = forceResponse * totalForce * deltaT;
	
	Vec3 localMoment = getCFrame().relativeToLocal(totalMoment);
	Vec3 localRotAcc = momentResponse * localMoment * deltaT;
	Vec3 rotAcc = getCFrame().localToRelative(localRotAcc);

	totalForce = Vec3();
	totalMoment = Vec3();

	motionOfCenterOfMass.translation.translation[0] += accel;
	motionOfCenterOfMass.rotation.rotation[0] += rotAcc;

	Vec3 oldCenterOfMass = this->totalCenterOfMass;
	Vec3 angularMomentumBefore = getTotalAngularMomentum();

	updateConstraints(deltaT);
	refreshPhysicalProperties();

	Vec3 deltaCOM = this->totalCenterOfMass - oldCenterOfMass;
	Vec3 movementOfCenterOfMass = motionOfCenterOfMass.getVelocity() * deltaT + accel * deltaT * deltaT * 0.5 - getCFrame().localToRelative(deltaCOM);

	rotateAroundCenterOfMass(Rotation::fromRotationVec(motionOfCenterOfMass.getAngularVelocity() * deltaT));
	translateUnsafeRecursive(movementOfCenterOfMass);

	Vec3 angularMomentumAfter = getTotalAngularMomentum();

	SymmetricMat3 globalMomentResponse = getCFrame().getRotation().localToGlobal(momentResponse);

	Vec3 deltaAngularVelocity = globalMomentResponse * (angularMomentumAfter - angularMomentumBefore);
	this->motionOfCenterOfMass.rotation.rotation[0] -= deltaAngularVelocity;

	updateAttachedPhysicals();
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
	motionOfCenterOfMass.translation.translation[0] += forceResponse * impulse;
}
void MotorizedPhysical::applyImpulse(Vec3Relative origin, Vec3Relative impulse) {
	assert(isVecValid(origin));
	assert(isVecValid(impulse));
	Debug::logVector(getCenterOfMass() + origin, impulse, Debug::IMPULSE);
	motionOfCenterOfMass.translation.translation[0] += forceResponse * impulse;
	Vec3 angularImpulse = origin % impulse;
	applyAngularImpulse(angularImpulse);
}
void MotorizedPhysical::applyAngularImpulse(Vec3 angularImpulse) {
	assert(isVecValid(angularImpulse));
	Debug::logVector(getCenterOfMass(), angularImpulse, Debug::ANGULAR_IMPULSE);
	Vec3 localAngularImpulse = getCFrame().relativeToLocal(angularImpulse);
	Vec3 localRotAcc = momentResponse * localAngularImpulse;
	Vec3 rotAcc = getCFrame().localToRelative(localRotAcc);
	motionOfCenterOfMass.rotation.rotation[0] += rotAcc;
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
	rotateAroundCenterOfMass(Rotation::fromRotationVec(rotAcc));
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

WorldPrototype* Physical::getWorld() const {
	return this->rigidBody.mainPart->getWorld();
}

double Physical::getVelocityKineticEnergy() const {
	return rigidBody.mass * lengthSquared(getMotionOfCenterOfMass().getVelocity()) / 2;
}
double Physical::getAngularKineticEnergy() const {
	Vec3 localAngularVel = getCFrame().relativeToLocal(getMotionOfCenterOfMass().getAngularVelocity());
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
Vec3 Physical::localToMainCOMRelative(const Vec3Local& vec) const {
	Position globalPos = this->getCFrame().localToGlobal(vec);
	return mainPhysical->getCFrame().globalToRelative(globalPos) - mainPhysical->totalCenterOfMass;
}

/*Motion MotorizedPhysical::getMotionOfCenterOfMass() const {
	return this->motionOfCenterOfMass;
}*/

Vec3 MotorizedPhysical::getVelocityOfCenterOfMass() const {
	return this->motionOfCenterOfMass.getVelocity();
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
	return connectionToParent.getRelativeCFrameToParent();
}

Position MotorizedPhysical::getCenterOfMass() const {
	return getCFrame().localToGlobal(totalCenterOfMass);
}

GlobalCFrame MotorizedPhysical::getCenterOfMassCFrame() const {
	return GlobalCFrame(getCFrame().localToGlobal(totalCenterOfMass), getCFrame().getRotation());
}

Vec3 MotorizedPhysical::getTotalImpulse() const {
	return this->motionOfCenterOfMass.getVelocity() * this->totalMass;
}

Vec3 MotorizedPhysical::getTotalAngularMomentum() const {
	Rotation selfRot = this->getCFrame().getRotation();

	ALLOCA_COMMotionTree(cache, this, size);

	SymmetricMat3 totalInertia = selfRot.localToGlobal(cache.getInertia());
	Vec3 localInternalAngularMomentum = cache.getInternalAngularMomentum();
	Vec3 globalInternalAngularMomentum = selfRot.localToGlobal(localInternalAngularMomentum);
	
	Vec3 externalAngularMomentum = totalInertia * this->motionOfCenterOfMass.getAngularVelocity();

	return externalAngularMomentum + globalInternalAngularMomentum;
}

Motion Physical::getMotion() const {
	if(this->isMainPhysical()) {
		return ((MotorizedPhysical*) this)->getMotion();
	} else {
		return ((ConnectedPhysical*) this)->getMotion();
	}
}

Motion MotorizedPhysical::getMotion() const {
	ALLOCA_COMMotionTree(cache, this, size);

	GlobalCFrame cf = this->getCFrame();
	TranslationalMotion motionOfCom = localToGlobal(cf.getRotation(), cache.motionOfCenterOfMass);

	return -motionOfCom + motionOfCenterOfMass.getMotionOfPoint(cf.localToRelative(-cache.centerOfMass));
}
Motion ConnectedPhysical::getMotion() const {
	// All motion and offset variables here are expressed in the global frame

	Motion parentMotion = parent->getMotion();

	RelativeMotion motionBetweenParentAndChild = connectionToParent.getRelativeMotion();

	RelativeMotion inGlobalFrame = motionBetweenParentAndChild.extendBegin(CFrame(parent->getCFrame().getRotation()));

	Motion result = inGlobalFrame.applyTo(parentMotion);

	return result;
}

Motion Physical::getMotionOfCenterOfMass() const {
	return this->getMotion().getMotionOfPoint(this->getCFrame().localToRelative(this->rigidBody.localCenterOfMass));
}
Motion MotorizedPhysical::getMotionOfCenterOfMass() const {
	return this->motionOfCenterOfMass;
}
Motion ConnectedPhysical::getMotionOfCenterOfMass() const {
	return this->getMotion().getMotionOfPoint(this->getCFrame().localToRelative(this->rigidBody.localCenterOfMass));
}

size_t Physical::getNumberOfPhysicalsInThisAndChildren() const {
	size_t totalPhysicals = 1;
	for(const ConnectedPhysical& child : childPhysicals) {
		totalPhysicals += child.getNumberOfPhysicalsInThisAndChildren();
	}
	return totalPhysicals;
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

std::tuple<double, Vec3, TranslationalMotion> InternalMotionTree::getInternalMotionOfCenterOfMass() const {
	TranslationalMotion totalMotion(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 0.0, 0.0));
	Vec3 totalCenterOfMass = this->motorPhys->rigidBody.localCenterOfMass * motorPhys->rigidBody.mass;
	double totalMass = motorPhys->rigidBody.mass;
	motorPhys->mutuallyRecurse(relativeMotionTree, [&totalMass, &totalCenterOfMass, &totalMotion](const MonotonicTreeNode<RelativeMotion>& node, const ConnectedPhysical& conPhys) {
		double mass = conPhys.rigidBody.mass;
		totalMotion += node.value.relativeMotion.translation * mass;
		totalCenterOfMass += node.value.locationOfRelativeMotion.getPosition() * mass;
		totalMass += mass;
	});

	totalCenterOfMass *= (1 / totalMass);
	totalMotion *= (1 / totalMass);

	return std::make_tuple(totalMass, totalCenterOfMass, totalMotion);
}

COMMotionTree InternalMotionTree::normalizeCenterOfMass() && {
	std::tuple<double, Vec3, TranslationalMotion> m = this->getInternalMotionOfCenterOfMass();

	for(MonotonicTreeNode<RelativeMotion>& elem : relativeMotionTree) {
		elem.value.locationOfRelativeMotion -= std::get<1>(m);
		elem.value.relativeMotion.translation -= std::get<2>(m);
	}

	return COMMotionTree(this->motorPhys, std::move(this->relativeMotionTree), std::get<0>(m), std::get<1>(m), std::get<2>(m));
}

SymmetricMat3 COMMotionTree::getInertia() const {
	SymmetricMat3 totalInertia = getTranslatedInertiaAroundCenterOfMass(motorPhys->rigidBody.inertia, motorPhys->rigidBody.mass, this->mainCOMOffset);
	motorPhys->mutuallyRecurse(relativeMotionTree, [&totalInertia](const MonotonicTreeNode<RelativeMotion>& node, const ConnectedPhysical& conPhys) {
		totalInertia += getTransformedInertiaAroundCenterOfMass(conPhys.rigidBody.inertia, conPhys.rigidBody.mass, node.value.locationOfRelativeMotion);
	});

	return totalInertia;
}

FullTaylor<SymmetricMat3> COMMotionTree::getInertiaDerivatives() const {
	FullTaylor<SymmetricMat3> totalInertia = getTranslatedInertiaDerivativesAroundCenterOfMass(motorPhys->rigidBody.inertia, motorPhys->rigidBody.mass, this->mainCOMOffset, -this->motionOfCenterOfMass);
	motorPhys->mutuallyRecurse(relativeMotionTree, [&totalInertia](const MonotonicTreeNode<RelativeMotion>& node, const ConnectedPhysical& conPhys) {
		totalInertia += getTransformedInertiaDerivativesAroundCenterOfMass(conPhys.rigidBody.inertia, conPhys.rigidBody.mass, node.value.locationOfRelativeMotion, node.value.relativeMotion);
	});

	return totalInertia;
}

Motion COMMotionTree::getMotion() const {
	GlobalCFrame cf = this->motorPhys->getCFrame();
	TranslationalMotion motionOfCom = localToGlobal(cf.getRotation(), this->motionOfCenterOfMass);

	return -motionOfCom + this->motorPhys->motionOfCenterOfMass.getMotionOfPoint(cf.localToRelative(-this->centerOfMass));
}

Vec3 COMMotionTree::getInternalAngularMomentum() const {
	Vec3 velocityOfMain = -motionOfCenterOfMass.getVelocity();
	Vec3 totalAngularMomentum = getAngularMomentumFromOffsetOnlyVelocity(this->mainCOMOffset, velocityOfMain, motorPhys->rigidBody.mass);

	motorPhys->mutuallyRecurse(relativeMotionTree, [&totalAngularMomentum](const MonotonicTreeNode<RelativeMotion>& node, const ConnectedPhysical& conPhys) {
		const RelativeMotion& relMotion = node.value;
		SymmetricMat3 relativeInertia = relMotion.locationOfRelativeMotion.getRotation().localToGlobal(conPhys.rigidBody.inertia);

		totalAngularMomentum += getAngularMomentumFromOffset(
			relMotion.locationOfRelativeMotion.getPosition(),
			relMotion.relativeMotion.getVelocity(),
			relMotion.relativeMotion.getAngularVelocity(),
			relativeInertia,
			conPhys.rigidBody.mass);
	});

	return totalAngularMomentum;
}

#pragma endregion

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
	assert(std::isfinite(totalMass));
	assert(isVecValid(totalCenterOfMass));
	assert(isMatValid(forceResponse));
	assert(isMatValid(momentResponse));

	return true;
}

bool ConnectedPhysical::isValid() const {
	assert(Physical::isValid());

	assert(isCFrameValid(connectionToParent.attachOnParent));
	assert(isCFrameValid(connectionToParent.attachOnChild));

	return true;
}

#pragma endregion

std::vector<FoundLayerRepresentative> findAllLayersIn(MotorizedPhysical* phys) {
	std::vector<FoundLayerRepresentative> result;

	phys->forEachPart([&result](Part& p) {
		for(FoundLayerRepresentative& item : result) {
			if(item.layer == p.layer) {
				return;
			}
		}
		result.push_back(FoundLayerRepresentative{p.layer, &p});
	});

	return result;
}

std::vector<FoundLayerRepresentative> findAllLayersIn(Part* part) {
	if(part->layer != nullptr) {
		if(part->parent != nullptr) {
			return findAllLayersIn(part->parent->mainPhysical);
		} else {
			return std::vector<FoundLayerRepresentative>{FoundLayerRepresentative{part->layer, part}};
		}
	} else {
		return std::vector<FoundLayerRepresentative>{};
	}
}
};