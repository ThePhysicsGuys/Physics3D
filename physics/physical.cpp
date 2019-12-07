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

Physical::Physical(Part* part, MotorizedPhysical* mainPhysical) : mainPart(part), mainPhysical(mainPhysical) {
	if (part->parent != nullptr) {
		throw "Attempting to re-add part to different physical!";
	}
	part->parent = this;

	refreshWithNewParts();
}

Physical::Physical(Physical&& other) noexcept :
	mainPart(std::move(other.mainPart)),
	parts(std::move(other.parts)),
	velocity(other.velocity),
	angularVelocity(other.angularVelocity),
	mass(other.mass),
	localCenterOfMass(other.localCenterOfMass),
	inertia(other.inertia) {
	mainPart->parent = this;
	for(AttachedPart& p : parts) {
		p.part->parent = this;
	}
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

Physical& Physical::operator=(Physical&& other) noexcept {
	//this->cframe = other.cframe;
	this->mainPart = other.mainPart;
	this->parts = std::move(other.parts);
	this->velocity = other.velocity;
	this->angularVelocity = other.angularVelocity;
	this->mass = other.mass;
	this->localCenterOfMass = other.localCenterOfMass;
	this->inertia = other.inertia;

	mainPart->parent = this;
	for(AttachedPart& p : parts) {
		p.part->parent = this;
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

bool liesInVector(const std::vector<AttachedPart>& vec, const AttachedPart* ptr) {
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

void Physical::attachPhysical(Physical* phys, const CFrame& attachment) {
	size_t originalAttachCount = this->parts.size();
	const GlobalCFrame& cf = this->getCFrame();
	phys->mainPart->cframe = cf.localToGlobal(attachment);
	this->parts.push_back(AttachedPart{attachment, phys->mainPart});
	phys->mainPart->parent = this;

	for (AttachedPart& ap : phys->parts) {
		CFrame globalAttach = attachment.localToGlobal(ap.attachment);
		this->parts.push_back(AttachedPart{globalAttach, ap.part});
		ap.part->cframe = cf.localToGlobal(globalAttach);
		ap.part->parent = this;
	}

	if (phys->world != nullptr) {
		phys->world->removePhysical(phys);
	}
	phys->mainPart = nullptr;
	phys->parts.clear();
	delete phys;
	if (this->world != nullptr) {
		NodeStack stack = world->objectTree.findGroupFor(this->mainPart, this->mainPart->getStrictBounds());
		TreeNode& group = **stack;
		for (size_t i = originalAttachCount; i < this->parts.size(); i++) {
			Part* p = parts[i].part;
			this->world->objectTree.addToExistingGroup(p, p->getStrictBounds(), group);
		}
		stack.expandBoundsAllTheWayToTop();
	}
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
			if (this->world != nullptr) {
				this->world->updatePartBounds(part, oldBounds);
			}
		} else {
			// attach other part's entire physical
			if (part->parent->mainPart == part) {
				attachPhysical(part->parent, attachment);
			} else {
				CFrame newAttach = attachment.localToGlobal(~part->parent->getAttachFor(part).attachment);
				attachPhysical(part->parent, newAttach);
			}
		}
	} else {
		part->parent = this;
		parts.push_back(AttachedPart{ attachment, part });
		part->cframe = getCFrame().localToGlobal(attachment);

		if (this->world != nullptr) {
			this->world->objectTree.addToExistingGroup(part, part->getStrictBounds(), this->mainPart, this->mainPart->getStrictBounds());
		}
	}

	refreshWithNewParts();
}

// detaches part, without updating World
// Returns true if this physical has no more parts and should be deleted
void Physical::detachPart(Part* part) {
	if (part == mainPart) {
		if (parts.size() == 0) {
			mainPart = nullptr;
		} else {
			makeMainPart(parts[parts.size() - 1]);
		}
		part->parent = nullptr;
		return;
	}
	for (signed long long i = parts.size() - 1; i >= 0; i--) {
		AttachedPart& at = parts[i];
		if (at.part == part) {
			part->parent = nullptr;
			parts.erase(parts.begin() + i);
			if (this->world != nullptr) {
				this->world->removePartFromTrees(part);
			}
			refreshWithNewParts();
			return;
		}
	}
	throw "Error: could not find part to remove!";
}

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

	if (this->anchored) {
		this->mainPhysical->forceResponse = SymmetricMat3::ZEROS();
		this->mainPhysical->momentResponse = SymmetricMat3::ZEROS();
	} else {
		this->mainPhysical->forceResponse = SymmetricMat3::IDENTITY() * (1/mass);
		this->mainPhysical->momentResponse = ~inertia;
	}
}

void Physical::updatePart(const Part* updatedPart, const Bounds& updatedBounds) {
	refreshWithNewParts();
	if (this->world != nullptr) {
		this->world->updatePartBounds(updatedPart, updatedBounds);
	}
}

BoundingBox Physical::computeLocalBounds() const {
	BoundingBox best(mainPart->hitbox.getBounds());

	for (const AttachedPart& p : parts) {
		double xmax = p.attachment.localToGlobal(p.part->hitbox.furthestInDirection(p.attachment.relativeToLocal(Vec3(1, 0, 0)))).x;
		double xmin = p.attachment.localToGlobal(p.part->hitbox.furthestInDirection(p.attachment.relativeToLocal(Vec3(-1, 0, 0)))).x;
		double ymax = p.attachment.localToGlobal(p.part->hitbox.furthestInDirection(p.attachment.relativeToLocal(Vec3(0, 1, 0)))).y;
		double ymin = p.attachment.localToGlobal(p.part->hitbox.furthestInDirection(p.attachment.relativeToLocal(Vec3(0, -1, 0)))).y;
		double zmax = p.attachment.localToGlobal(p.part->hitbox.furthestInDirection(p.attachment.relativeToLocal(Vec3(0, 0, 1)))).z;
		double zmin = p.attachment.localToGlobal(p.part->hitbox.furthestInDirection(p.attachment.relativeToLocal(Vec3(0, 0, -1)))).z;

		best.xmax = std::max(best.xmax, xmax);
		best.xmin = std::min(best.xmin, xmin);
		best.ymax = std::max(best.ymax, ymax);
		best.ymin = std::min(best.ymin, ymin);
		best.zmax = std::max(best.zmax, zmax);
		best.zmin = std::min(best.zmin, zmin);
	}

	return best;
}

Bounds Physical::getStrictBounds() const {
	Bounds bounds = mainPart->getStrictBounds();

	for (const AttachedPart& p : parts) {
		bounds = unionOfBounds(bounds, p.part->getStrictBounds());
	}

	return bounds;
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
	world->updatePartGroupBounds(this->mainPart, oldBounds);
}
void Physical::translate(const Vec3& translation) {
	Bounds oldBounds = this->mainPart->getStrictBounds();
	translateUnsafe(translation);
	updateAttachedPartCFrames();
	world->updatePartGroupBounds(this->mainPart, oldBounds);
}

void MotorizedPhysical::update(double deltaT) {
	Vec3 accel = forceResponse * totalForce * deltaT;
	
	Vec3 localMoment = getCFrame().relativeToLocal(totalMoment);
	Vec3 localRotAcc = momentResponse * localMoment * deltaT;
	Vec3 rotAcc = getCFrame().localToRelative(localRotAcc);

	totalForce = Vec3();
	totalMoment = Vec3();

	velocity += accel;
	angularVelocity += rotAcc;

	Vec3 movement = velocity * deltaT + accel * deltaT * deltaT / 2;

	Mat3 rotation = fromRotationVec(angularVelocity * deltaT);

	rotateAroundCenterOfMassUnsafe(rotation);
	translateUnsafe(movement);

	updateAttachedPartCFrames();
}

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
	velocity += forceResponse * impulse;
}
void MotorizedPhysical::applyImpulse(Vec3Relative origin, Vec3Relative impulse) {
	CHECK_VALID_VEC(origin);
	CHECK_VALID_VEC(impulse);
	Debug::logVector(getCenterOfMass() + origin, impulse, Debug::IMPULSE);
	velocity += forceResponse * impulse;
	Vec3 angularImpulse = origin % impulse;
	applyAngularImpulse(angularImpulse);
}
void MotorizedPhysical::applyAngularImpulse(Vec3 angularImpulse) {
	CHECK_VALID_VEC(angularImpulse);
	Debug::logVector(getCenterOfMass(), angularImpulse, Debug::ANGULAR_IMPULSE);
	Vec3 localAngularImpulse = getCFrame().relativeToLocal(angularImpulse);
	Vec3 localRotAcc = momentResponse * localAngularImpulse;
	Vec3 rotAcc = getCFrame().localToRelative(localRotAcc);
	angularVelocity += rotAcc;
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

Position Physical::getCenterOfMass() const {
	return getCFrame().localToGlobal(localCenterOfMass);
}
// gets velocity of point relative to center of mass
Vec3 Physical::getVelocityOfPoint(const Vec3Relative& point) const {
	return velocity + angularVelocity % point;
}

Vec3 MotorizedPhysical::getAcceleration() const {
	return forceResponse * totalForce;
}

Vec3 MotorizedPhysical::getAngularAcceleration() const {
	return momentResponse * getCFrame().relativeToLocal(totalMoment);
}

Vec3 Physical::getAccelerationOfPoint(const Vec3Relative& point) const {
	return getAcceleration() + getAngularAcceleration() % point + angularVelocity % (angularVelocity % point);
}

void Physical::setCFrameUnsafe(const GlobalCFrame& newCFrame) {
	this->mainPart->cframe = newCFrame;
	for (const AttachedPart& p : parts) {
		p.part->cframe = newCFrame.localToGlobal(p.attachment);
	}
}

void Physical::setCFrame(const GlobalCFrame& newCFrame) {
	if (this->world != nullptr) {
		Bounds oldMainPartBounds = this->mainPart->getStrictBounds();

		setCFrameUnsafe(newCFrame);

		world->updatePartGroupBounds(this->mainPart, oldMainPartBounds);
	} else {
		setCFrameUnsafe(newCFrame);
	}
}

void Physical::setPartCFrame(Part* part, const GlobalCFrame& newCFrame) {
	if (part == mainPart) {
		setCFrame(newCFrame);
	} else {
		CFrame attach = getAttachFor(part).attachment;
		GlobalCFrame newMainCFrame = newCFrame.localToGlobal(~attach);

		setCFrame(newMainCFrame);
	}
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

double Physical::getVelocityKineticEnergy() const {
	return mass * lengthSquared(velocity) / 2;
}
double Physical::getAngularKineticEnergy() const {
	Vec3 localAngularVel = getCFrame().relativeToLocal(angularVelocity);
	return (inertia * localAngularVel) * localAngularVel / 2;
}
double Physical::getKineticEnergy() const {
	return getVelocityKineticEnergy() + getAngularKineticEnergy();
}

ConnectedPhysical::ConnectedPhysical(Physical&& phys, HardConstraint* constraintWithParent, MotorizedPhysical* mainPhysical) :
	Physical(std::move(phys)), constraintWithParent(constraintWithParent) {
	this->mainPhysical = mainPhysical;
}

MotorizedPhysical::MotorizedPhysical(Part* mainPart) : Physical(mainPart, this) {}

Vec3 Physical::getAcceleration() const { return mainPhysical->getAcceleration(); }
Vec3 Physical::getAngularAcceleration() const { return mainPhysical->getAngularAcceleration(); }

void Physical::applyForceAtCenterOfMass(Vec3 force) { mainPhysical->applyForceAtCenterOfMass(force); }
void Physical::applyForce(Vec3Relative origin, Vec3 force) { mainPhysical->applyForce(origin, force); }
void Physical::applyMoment(Vec3 moment) { mainPhysical->applyMoment(moment); }
void Physical::applyImpulseAtCenterOfMass(Vec3 impulse) { mainPhysical->applyImpulseAtCenterOfMass(impulse); }
void Physical::applyImpulse(Vec3Relative origin, Vec3Relative impulse) { mainPhysical->applyImpulse(origin, impulse); }
void Physical::applyAngularImpulse(Vec3 angularImpulse) { mainPhysical->applyAngularImpulse(angularImpulse); }
void Physical::applyDragAtCenterOfMass(Vec3 drag) { mainPhysical->applyDragAtCenterOfMass(drag); }
void Physical::applyDrag(Vec3Relative origin, Vec3Relative drag) { mainPhysical->applyDrag(origin, drag); }
void Physical::applyAngularDrag(Vec3 angularDrag) { mainPhysical->applyAngularDrag(angularDrag); }

SymmetricMat3 Physical::getResponseMatrix(const Vec3Local& localPoint) const {
	return mainPhysical->getResponseMatrix(localPoint);
}
Mat3 Physical::getResponseMatrix(const Vec3Local& actionPoint, const Vec3Local& responsePoint) const {
	return mainPhysical->getResponseMatrix(actionPoint, responsePoint);
}
double Physical::getInertiaOfPointInDirectionLocal(const Vec3Local& localPoint, const Vec3Local& localDirection) const {
	return mainPhysical->getInertiaOfPointInDirectionLocal(localPoint, localDirection);
}
double Physical::getInertiaOfPointInDirectionRelative(const Vec3Relative& relativePoint, const Vec3Relative& relativeDirection) const 							{
	return mainPhysical->getInertiaOfPointInDirectionRelative(relativePoint, relativeDirection);
}