#pragma once

#include "math/linalg/vec.h"
#include "math/linalg/mat.h"
#include "math/cframe.h"
#include "math/bounds.h"
#include "math/position.h"
#include "math/globalCFrame.h"

#include <vector>

#include "datastructures/iteratorEnd.h"

#include "part.h"
#include "constraints/hardConstraint.h"

typedef Vec3 Vec3Local;
typedef Vec3 Vec3Relative;

struct AttachedPart {
	CFrame attachment = CFrame();
	Part* part = nullptr;
};

struct PartIter {
	AttachedPart* iter;
	AttachedPart* iterEnd;
	Part* first;

	PartIter() = default;
	PartIter(AttachedPart* iter, AttachedPart* iterEnd, Part* first) : iter(iter-1), iterEnd(iterEnd), first(first) {}

	inline Part& operator*() const {
		return (first != nullptr) ? *first : *iter->part;
	}
	inline void operator++() {
		++iter;
		first = nullptr;
	}
	inline bool operator!=(IteratorEnd) const { return this->iter != this->iterEnd; }
	inline bool operator==(IteratorEnd) const { return this->iter == this->iterEnd; }
};
struct ConstPartIter {
	const AttachedPart* iter;
	const AttachedPart* iterEnd;
	const Part* first;

	ConstPartIter() = default;
	ConstPartIter(const AttachedPart* iter, const AttachedPart* iterEnd, const Part* first) : iter(iter - 1), iterEnd(iterEnd), first(first) {}

	inline const Part& operator*() const {
		return (first != nullptr) ? *first : *iter->part;
	}
	inline void operator++() {
		++iter;
		first = nullptr;
	}
	inline bool operator!=(IteratorEnd) const { return this->iter != this->iterEnd; }
	inline bool operator==(IteratorEnd) const { return this->iter == this->iterEnd; }
};

class WorldPrototype;

class ConnectedPhysical;
class MotorizedPhysical;

class Physical {
	friend class WorldPrototype;
	friend class Part;
protected:
	void updateAttachedPartCFrames();
	void updateAttachedPhysicals(double deltaT);
	void translateUnsafeRecursive(const Vec3Fix& translation);
	void setCFrameUnsafe(const GlobalCFrame& newCFrame);


	void refreshWithNewParts();

	// deletes the given physical
	void attachPhysical(Physical&& phys, const CFrame& attachment);

	void attachPhysical(Physical&& phys, HardConstraint* constraint, const CFrame& attachToThis, const CFrame& attachToThat);

	void attachPart(Part* part, HardConstraint* constraint, const CFrame& attachToThis, const CFrame& attachToThat);
	
public:
	Part* mainPart;
	std::vector<AttachedPart> parts;

	double mass;
	Vec3 localCenterOfMass;
	SymmetricMat3 inertia;

	MotorizedPhysical* mainPhysical;
	std::vector<ConnectedPhysical> childPhysicals;

	bool anchored = false;

	Physical() = default;
	Physical(Part* mainPart, MotorizedPhysical* mainPhysical);

	Physical(Physical&& other) noexcept;
	Physical& operator=(Physical&& other) noexcept;
	Physical(const Physical&) = delete;
	void operator=(const Physical&) = delete;

	~Physical();

	void updatePart(const Part* updatedPart, const Bounds& updatedBounds);

	const AttachedPart& getAttachFor(const Part* attachedPart) const {
		for (const AttachedPart& p : parts) {
			if (p.part == attachedPart)
				return p;
		}

		throw "Part not in this physical!";
	}

	AttachedPart& getAttachFor(const Part* attachedPart) {
		for (AttachedPart& p : parts) {
			if (p.part == attachedPart)
				return p;
		}

		throw "Part not in this physical!";
	}

	void setCFrame(const GlobalCFrame& newCFrame);
	void setPartCFrame(Part* part, const GlobalCFrame& newCFrame);
	inline const GlobalCFrame& getCFrame() const { return mainPart->getCFrame(); }

	inline Position getPosition() const {
		return getCFrame().getPosition();
	}

	Position getObjectCenterOfMass() const;
	GlobalCFrame getObjectCenterOfMassCFrame() const;
	Motion getMotion() const;
	
	double getVelocityKineticEnergy() const;
	double getAngularKineticEnergy() const;
	double getKineticEnergy() const;
	inline size_t getPartCount() const { return parts.size() + 1; }

	inline void setAnchored(bool anchored) { this->anchored = anchored; refreshWithNewParts(); }

	void applyDragToPhysical(Vec3 origin, Vec3 drag);
	void applyForceToPhysical(Vec3 origin, Vec3 force);
	void applyImpulseToPhysical(Vec3 origin, Vec3 impulse);

	CFrame getRelativeCFrameToMain() const;
	Vec3 localToMain(const Vec3Local& vec) const;

	void makeMainPart(AttachedPart& newMainPart);
	void makeMainPart(Part* newMainPart);
	void attachPart(Part* part, const CFrame& attachment);
	void detachPart(Part* part);



	PartIter begin() { return PartIter(parts.begin()._Ptr, parts.begin()._Ptr + parts.size(), mainPart); }
	ConstPartIter begin() const { return ConstPartIter(parts.begin()._Ptr, parts.begin()._Ptr + parts.size(), mainPart);}

	IteratorEnd end() const { return IteratorEnd(); }

	const Part& operator[](size_t index) const { return (index == 0) ? *mainPart : *parts[index - 1].part; }
	Part& operator[](size_t index) { return (index == 0) ? *mainPart : *parts[index - 1].part; }

	bool isValid() const;

	bool isMainPhysical() const;
	void removeChild(ConnectedPhysical* child);
};


class ConnectedPhysical : public Physical {
	friend class Physical;
	CFrame attachOnParent;
	CFrame attachOnThis;
	HardConstraint* constraintWithParent;
	Physical* parent;
public:
	ConnectedPhysical(Physical&& phys, Physical* parent, HardConstraint* constraintWithParent, const CFrame& attachOnThis, const CFrame& attachOnParent);

	CFrame getRelativeCFrameToParent() const;

	void updateCFrame(const GlobalCFrame& parentCFrame);

	bool isValid() const;
};

class MotorizedPhysical : public Physical {
	void rotateAroundCenterOfMassUnsafe(const RotMat3& rotation);
public:
	void refreshPhysicalProperties();
	Vec3 totalForce = Vec3(0.0, 0.0, 0.0);
	Vec3 totalMoment = Vec3(0.0, 0.0, 0.0);

	double totalMass;
	Vec3 totalCenterOfMass;

	WorldPrototype* world = nullptr;
	
	SymmetricMat3 forceResponse;
	SymmetricMat3 momentResponse;

	Motion motionOfCenterOfMass;
	
	MotorizedPhysical(Part* mainPart);

	Position getCenterOfMass() const;
	GlobalCFrame getCenterOfMassCFrame() const;

	void ensureWorld(WorldPrototype* world);

	void update(double deltaT);

	void rotateAroundCenterOfMass(const RotMat3& rotation);
	void translate(const Vec3& translation);

	void applyForceAtCenterOfMass(Vec3 force);
	void applyForce(Vec3Relative origin, Vec3 force);
	void applyMoment(Vec3 moment);
	void applyImpulseAtCenterOfMass(Vec3 impulse);
	void applyImpulse(Vec3Relative origin, Vec3Relative impulse);
	void applyAngularImpulse(Vec3 angularImpulse);
	void applyDragAtCenterOfMass(Vec3 drag);
	void applyDrag(Vec3Relative origin, Vec3Relative drag);
	void applyAngularDrag(Vec3 angularDrag);

	SymmetricMat3 getResponseMatrix(const Vec3Local& localPoint) const;
	Mat3 getResponseMatrix(const Vec3Local& actionPoint, const Vec3Local& responsePoint) const;
	double getInertiaOfPointInDirectionLocal(const Vec3Local& localPoint, const Vec3Local& localDirection) const;
	double getInertiaOfPointInDirectionRelative(const Vec3Relative& relativePoint, const Vec3Relative& relativeDirection) const;

	bool isValid() const;
};
