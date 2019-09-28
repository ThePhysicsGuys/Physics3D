#include "physical.h"

#include "../util/log.h"

#include "world.h"
#include "math/linalg/mat.h"
#include "math/linalg/misc.h"
#include "math/linalg/trigonometry.h"

#include "debug.h"
#include <algorithm>
#include <limits>



Physical::Physical(Part* part) : mainPart(part) {
	if (part->parent != nullptr) {
		throw "Attempting to re-add part to different physical!";
	}
	part->parent = this;

	refreshWithNewParts();
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

void Physical::makeMainPart(AttachedPart& newMainPart) {
	if (parts.liesInList(&newMainPart)) {
		CFrame& newCenterCFrame = newMainPart.attachment;
		std::swap(mainPart, newMainPart.part);
		for (AttachedPart& atPart : parts) {
			if (&atPart != &newMainPart) {
				atPart.attachment = newCenterCFrame.globalToLocal(atPart.attachment);
			}
		}
		newCenterCFrame = ~newCenterCFrame;
		parts.size--;
	} else {
		throw "Attempting to make a part not in this physical the mainPart!";
	}
}

void Physical::attachPhysical(Physical* phys, const CFrame& attachment) {
	size_t originalAttachCount = this->parts.size;
	const GlobalCFrame& cf = this->getCFrame();
	phys->mainPart->cframe = cf.localToGlobal(attachment);
	this->parts.add(AttachedPart{attachment, phys->mainPart});
	phys->mainPart->parent = this;

	for (AttachedPart& ap : phys->parts) {
		CFrame globalAttach = attachment.localToGlobal(ap.attachment);
		this->parts.add(AttachedPart{globalAttach, ap.part});
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
		for (size_t i = originalAttachCount; i < this->parts.size; i++) {
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
		parts.add(AttachedPart{ attachment, part });
		part->cframe = getCFrame().localToGlobal(attachment);

		if (this->world != nullptr) {
			this->world->objectTree.addToExistingGroup(part, part->getStrictBounds(), this->mainPart, this->mainPart->getStrictBounds());
		}
	}

	refreshWithNewParts();
}
void Physical::detachPart(Part* part) {
	if (this->world != nullptr) {
		this->world->objectTree.remove(part);
	}
	if (part == mainPart) {
		if (parts.size == 0) {
			return;
		}
		part->parent = nullptr;
		if (parts.size >= 1) {
			makeMainPart(parts[parts.size-1]);
			refreshWithNewParts();
		} else {
			if (this->world != nullptr) {
				this->world->removePhysical(this);
			}
		}
	} else {
		for (auto iter = parts.begin(); iter != parts.end(); ++iter) {
			AttachedPart& at = *iter;
			if (at.part == part) {
				part->parent = nullptr;
				parts.remove(iter);
				refreshWithNewParts();
				return;
			}
		}
		throw "Error: could not find part to remove!";
	}
}

void Physical::refreshWithNewParts() {
	double totalMass = mainPart->mass;
	SymmetricMat3 totalInertia = mainPart->inertia;
	Vec3 totalCenterOfMass = mainPart->localCenterOfMass * mainPart->mass;
	for (const AttachedPart& p : parts) {
		totalMass += p.part->mass;
		totalCenterOfMass += p.attachment.localToGlobal(p.part->localCenterOfMass) * p.part->mass;
	}
	totalCenterOfMass /= totalMass;
	for (const AttachedPart& p : parts) {
		totalInertia += transformBasis(p.part->inertia, p.attachment.getRotation()) + skewSymmetricSquared(p.attachment.getPosition() - totalCenterOfMass) * p.part->mass;
	}
	this->mass = totalMass;
	this->localCenterOfMass = totalCenterOfMass;
	this->inertia = totalInertia;

	this->localBounds = computeLocalBounds();
	Sphere s = computeLocalCircumscribingSphere();
	this->localCentroid = s.origin;
	this->circumscribingSphere.radius = s.radius;
	this->circumscribingSphere.origin = getCFrame().localToGlobal(localCentroid);

	if (this->anchored) {
		this->forceResponse = SymmetricMat3::ZEROS();
		this->momentResponse = SymmetricMat3::ZEROS();
	} else {
		this->forceResponse = SymmetricMat3::IDENTITY() * (1/mass);
		this->momentResponse = ~inertia;
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

Sphere Physical::computeLocalCircumscribingSphere() const {
	BoundingBox b = computeLocalBounds();
	Vec3 localCentroid = b.getCenter();
	double maxRadiusSq = mainPart->hitbox.getMaxRadiusSq(localCentroid);
	for (const AttachedPart& p : parts) {
		double radiusSq = p.part->hitbox.getMaxRadiusSq(p.attachment.globalToLocal(localCentroid));
		maxRadiusSq = std::max(maxRadiusSq, radiusSq);
	}
	return Sphere{ localCentroid, sqrt(maxRadiusSq) };
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

	this->circumscribingSphere.origin = getCFrame().localToGlobal(localCentroid);
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

void Physical::update(double deltaT) {
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

void Physical::applyForceAtCenterOfMass(Vec3 force) {
	totalForce += force;

	Debug::logVector(getCenterOfMass(), force, Debug::FORCE);
}

void Physical::applyForce(Vec3Relative origin, Vec3 force) {
	totalForce += force;

	Debug::logVector(getCenterOfMass() + origin, force, Debug::FORCE);

	applyMoment(origin % force);
}

void Physical::applyMoment(Vec3 moment) {
	totalMoment += moment;
	Debug::logVector(getCenterOfMass(), moment, Debug::MOMENT);
}

void Physical::applyImpulseAtCenterOfMass(Vec3 impulse) {
	Debug::logVector(getCenterOfMass(), impulse, Debug::IMPULSE);
	velocity += forceResponse * impulse;
}
void Physical::applyImpulse(Vec3Relative origin, Vec3Relative impulse) {
	Debug::logVector(getCenterOfMass() + origin, impulse, Debug::IMPULSE);
	velocity += forceResponse * impulse;
	Vec3 angularImpulse = origin % impulse;
	applyAngularImpulse(angularImpulse);
}
void Physical::applyAngularImpulse(Vec3 angularImpulse) {
	Debug::logVector(getCenterOfMass(), angularImpulse, Debug::ANGULAR_IMPULSE);
	Vec3 localAngularImpulse = getCFrame().relativeToLocal(angularImpulse);
	Vec3 localRotAcc = momentResponse * localAngularImpulse;
	Vec3 rotAcc = getCFrame().localToRelative(localRotAcc);
	angularVelocity += rotAcc;
}

void Physical::applyDragAtCenterOfMass(Vec3 drag) {
	Debug::logVector(getCenterOfMass(), drag, Debug::POSITION);
	translate(forceResponse * drag);
}
void Physical::applyDrag(Vec3Relative origin, Vec3Relative drag) {
	Debug::logVector(getCenterOfMass() + origin, drag, Debug::POSITION);
	translateUnsafe(forceResponse * drag);
	Vec3 angularDrag = origin % drag;
	applyAngularDrag(angularDrag);
}
void Physical::applyAngularDrag(Vec3 angularDrag) {
	Debug::logVector(getCenterOfMass(), angularDrag, Debug::INFO_VEC);
	Vec3 localAngularDrag = getCFrame().relativeToLocal(angularDrag);
	Vec3 localRotAcc = momentResponse * localAngularDrag;
	Vec3 rotAcc = getCFrame().localToRelative(localRotAcc);
	rotateAroundCenterOfMassUnsafe(fromRotationVec(rotAcc));
}

Position Physical::getCenterOfMass() const {
	return getCFrame().localToGlobal(localCenterOfMass);
}
Position Physical::getCentroid() const {
	return getCFrame().localToGlobal(localCentroid);
}

Vec3 Physical::getVelocityOfPoint(const Vec3Relative& point) const {
	return velocity + angularVelocity % point;
}

Vec3 Physical::getAcceleration() const {
	return forceResponse * totalForce;
}

Vec3 Physical::getAngularAcceleration() const {
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

	this->circumscribingSphere.origin = getCFrame().localToGlobal(localCentroid);
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
SymmetricMat3 Physical::getResponseMatrix(const Vec3Local& r) const {
	Mat3 crossMat = createCrossProductEquivalent(r);

	SymmetricMat3 rotationFactor = multiplyLeftRight(momentResponse , crossMat);

	return forceResponse + rotationFactor;
}

Mat3 Physical::getResponseMatrix(const Vec3Local& actionPoint, const Vec3Local& responsePoint) const {
	Mat3 actionCross = createCrossProductEquivalent(actionPoint);
	Mat3 responseCross = createCrossProductEquivalent(responsePoint);

	Mat3 rotationFactor = responseCross * momentResponse * actionCross;

	return Mat3(forceResponse) - rotationFactor;
}
double Physical::getInertiaOfPointInDirectionLocal(const Vec3Local& localPoint, const Vec3Local& localDirection) const {
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

double Physical::getInertiaOfPointInDirectionRelative(const Vec3Relative& relPoint, const Vec3Relative& relDirection) const {
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
