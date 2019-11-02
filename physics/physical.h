#pragma once

#include "math/linalg/vec.h"
#include "math/linalg/mat.h"
#include "math/cframe.h"
#include "part.h"
#include "math/bounds.h"

#include "math/position.h"
#include "math/globalCFrame.h"

#include <vector>

#include "datastructures/iteratorEnd.h"


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

class Physical {
	void updateAttachedPartCFrames();
	void translateUnsafe(const Vec3& translation);
	void rotateAroundCenterOfMassUnsafe(const RotMat3& rotation);
	void setCFrameUnsafe(const GlobalCFrame& newCFrame);

	// deletes the given physical
	void attachPhysical(Physical* phys, const CFrame& attachment);
public:
	Part* mainPart;
	WorldPrototype* world = nullptr;
	std::vector<AttachedPart> parts;
	Vec3 velocity = Vec3();
	Vec3 angularVelocity = Vec3();

	Vec3 totalForce = Vec3();
	Vec3 totalMoment = Vec3();

	double mass;
	Vec3 localCenterOfMass;
	SymmetricMat3 inertia;

	SymmetricMat3 forceResponse;
	SymmetricMat3 momentResponse;

	bool anchored = false;

	Physical() = default;
	explicit Physical(Part* part);
	inline Physical(Part* part, double mass, SymmetricMat3 inertia) : mass(mass), inertia(inertia), mainPart(part) {
		//parts.push_back(AttachedPart{ CFrame(), part });
		
	}

	Physical(Physical&& other) noexcept : 
		mainPart(std::move(other.mainPart)), 
		parts(std::move(other.parts)), 
		forceResponse(other.forceResponse), 
		momentResponse(other.momentResponse),	
		velocity(other.velocity),
		angularVelocity(other.angularVelocity),
		totalForce(other.totalForce),
		totalMoment(other.totalMoment),
		mass(other.mass),
		localCenterOfMass(other.localCenterOfMass),
		inertia(other.inertia)
		{
		mainPart->parent = this;
		for (AttachedPart& p : parts) {
			p.part->parent = this;
		}
	}

	Physical& operator=(Physical&& other) noexcept {
		//this->cframe = other.cframe;
		this->mainPart = other.mainPart;
		this->parts = std::move(other.parts);
		this->velocity = other.velocity;
		this->angularVelocity = other.angularVelocity;
		this->totalForce = other.totalForce;
		this->totalMoment = other.totalMoment;
		this->mass = other.mass;
		this->localCenterOfMass = other.localCenterOfMass;
		this->inertia = other.inertia;

		mainPart->parent = this;
		for (AttachedPart& p : parts) {
			p.part->parent = this;
		}
		return *this;
	}

	Physical(const Physical&) = delete;
	void operator=(const Physical&) = delete;

	~Physical();

	void refreshWithNewParts();

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

	void update(double deltaT);
	void applyForceAtCenterOfMass(Vec3 force);
	void applyForce(Vec3Relative origin, Vec3 force);
	void applyMoment(Vec3 moment);
	void applyImpulseAtCenterOfMass(Vec3 impulse);
	void applyImpulse(Vec3Relative origin, Vec3Relative impulse);
	void applyAngularImpulse(Vec3 angularImpulse);
	void applyDragAtCenterOfMass(Vec3 drag);
	void applyDrag(Vec3Relative origin, Vec3Relative drag);
	void applyAngularDrag(Vec3 angularDrag);

	void rotateAroundCenterOfMass(const RotMat3& rotation);
	void translate(const Vec3& translation);

	void setCFrame(const GlobalCFrame& newCFrame);
	void setPartCFrame(Part* part, const GlobalCFrame& newCFrame);
	inline const GlobalCFrame& getCFrame() const { return mainPart->getCFrame(); }

	inline Position getPosition() const {
		return getCFrame().getPosition();
	}

	Bounds getStrictBounds() const;

	Position getCenterOfMass() const;
	Vec3 getAcceleration() const;
	Vec3 getAngularAcceleration() const;
	Vec3 getVelocityOfPoint(const Vec3Relative& point) const;
	Vec3 getAccelerationOfPoint(const Vec3Relative& point) const;
	SymmetricMat3 getResponseMatrix(const Vec3Local& localPoint) const;
	Mat3 getResponseMatrix(const Vec3Local& actionPoint, const Vec3Local& responsePoint) const;
	double getInertiaOfPointInDirectionLocal(const Vec3Local& localPoint, const Vec3Local& localDirection) const;
	double getInertiaOfPointInDirectionRelative(const Vec3Relative& relativePoint, const Vec3Relative& relativeDirection) const;
	double getVelocityKineticEnergy() const;
	double getAngularKineticEnergy() const;
	double getKineticEnergy() const;
	inline size_t getPartCount() const { return parts.size() + 1; }
	BoundingBox computeLocalBounds() const;

	inline void setAnchored(bool anchored) { this->anchored = anchored; refreshWithNewParts(); }
	


	void makeMainPart(AttachedPart& newMainPart);
	void makeMainPart(Part* newMainPart);
	void attachPart(Part* part, const CFrame& attachment);
	void detachPart(Part* part);



	PartIter begin() { return PartIter(parts.begin()._Ptr, parts.begin()._Ptr + parts.size(), mainPart); }
	ConstPartIter begin() const { return ConstPartIter(parts.begin()._Ptr, parts.begin()._Ptr + parts.size(), mainPart);}

	IteratorEnd end() const { return IteratorEnd(); }

	const Part& operator[](size_t index) const { return (index == 0) ? *mainPart : *parts[index - 1].part; }
	Part& operator[](size_t index) { return (index == 0) ? *mainPart : *parts[index - 1].part; }
};
