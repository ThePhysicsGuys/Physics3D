#pragma once

struct Physical;

#include "math/mat3.h"
#include "math/vec3.h"
#include "math/cframe.h"
#include "part.h"

#include <vector>
#include "datastructures/splitUnorderedList.h"

typedef Vec3 Vec3Local;
typedef Vec3 Vec3Relative;

struct AttachedPart {
	CFrame attachment;
	Part* part;
};

struct PartIter {
	AttachedPart* iter;
	Part* first;

	PartIter(AttachedPart* iter, Part* first) : iter(iter), first(first) {}

	inline Part& operator*() const {
		return (first != nullptr)? *first : *iter->part;
	}
	inline void operator++() {
		++iter;
		first = nullptr;
	}
	inline bool operator!=(const PartIter& other) const { return this->iter != other.iter; }
	inline bool operator==(const PartIter& other) const { return this->iter == other.iter; }
};
struct ConstPartIter {
	const AttachedPart* iter;
	const Part* first;

	ConstPartIter(const AttachedPart* iter, Part* first) : iter(iter), first(first) {}

	inline const Part& operator*() const {
		return (first != nullptr) ? *first : *iter->part;
	}
	inline void operator++() {
		++iter;
		first = nullptr;
	}
	inline bool operator!=(const ConstPartIter& other) const { return this->iter != other.iter; }
	inline bool operator==(const ConstPartIter& other) const { return this->iter == other.iter; }
};

class Physical {
public:
	// CFrame cframe;
	Part* mainPart;
	SplitUnorderedList<AttachedPart> parts;
	Vec3 velocity = Vec3();
	Vec3 angularVelocity = Vec3();

	Vec3 totalForce = Vec3();
	Vec3 totalMoment = Vec3();

	double mass;
	Vec3 localCenterOfMass;
	SymmetricMat3 inertia;

	Vec3 localCentroid;
	Sphere circumscribingSphere;

	Physical() = default;
	Physical(Part* part);
	inline Physical(Part* part, double mass, SymmetricMat3 inertia) : mass(mass), inertia(inertia), mainPart(part) {
		//parts.push_back(AttachedPart{ CFrame(), part });
		
		Sphere smallestSphere = part->hitbox.getCircumscribingSphere();
		Sphere s = getLocalCircumscribingSphere();
		this->localCentroid = s.origin;
		this->circumscribingSphere.radius = s.radius;
		this->circumscribingSphere.origin = getCFrame().localToGlobal(localCentroid);
	}

	Physical(Physical&& other) noexcept : parts(std::move(other.parts)) {
		//this->cframe = other.cframe;
		this->mainPart = other.mainPart;
		this->velocity = other.velocity;
		this->angularVelocity = other.angularVelocity;
		this->totalForce = other.totalForce;
		this->totalMoment = other.totalMoment;
		this->mass = other.mass;
		this->localCenterOfMass = other.localCenterOfMass;
		this->inertia = other.inertia;
		this->localCentroid = other.localCentroid;
		this->circumscribingSphere = other.circumscribingSphere;

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
		this->localCentroid = other.localCentroid;
		this->circumscribingSphere = other.circumscribingSphere;

		mainPart->parent = this;
		for (AttachedPart& p : parts) {
			p.part->parent = this;
		}
		return *this;
	}

	Physical(const Physical&) = delete;
	void operator=(const Physical&) = delete;

	// bad constructor
	/*~Physical() {
		mainPart->parent = nullptr;
		mainPart = nullptr;
		for (AttachedPart& atPart:parts) {
			atPart.part->parent = nullptr;
		}
	}*/

	void makeMainPart(AttachedPart& newMainPart);
	void makeMainPart(Part* newMainPart);
	void attachPart(Part* part, CFrame attachment);
	void detachPart(Part* part);
	void refreshWithNewParts();


	void update(double deltaT);
	void applyForceAtCenterOfMass(Vec3 force);
	void applyForce(Vec3Relative origin, Vec3 force);
	void applyMoment(Vec3 moment);
	void applyImpulseAtCenterOfMass(Vec3 impulse);
	void applyImpulse(Vec3Relative origin, Vec3Relative impulse);
	void applyAngularImpulse(Vec3 angularImpulse);

	void rotateAroundCenterOfMass(const RotMat3& rotation);
	void translate(const Vec3& translation);

	void setCFrame(const CFrame& newCFrame);
	inline const CFrame& getCFrame() const { return mainPart->cframe; }

	Vec3 getCenterOfMass() const;
	Vec3 getAcceleration() const;
	Vec3 getAngularAcceleration() const;
	Vec3 getVelocityOfPoint(const Vec3Relative& point) const;
	Vec3 getAccelerationOfPoint(const Vec3Relative& point) const;
	SymmetricMat3 getPointAccelerationMatrix(const Vec3Local& localPoint) const;
	double getInertiaOfPointInDirectionLocal(const Vec3Local& localPoint, const Vec3Local& localDirection) const;
	double getInertiaOfPointInDirectionRelative(const Vec3Relative& relativePoint, const Vec3Relative& relativeDirection) const;
	double getVelocityKineticEnergy() const;
	double getAngularKineticEnergy() const;
	double getKineticEnergy() const;
	inline size_t getPartCount() const { return parts.size + 1; }
	BoundingBox getLocalBounds() const;
	Sphere getLocalCircumscribingSphere() const;

	PartIter begin() { return PartIter(parts.begin()-1, mainPart); }
	ConstPartIter begin() const { return ConstPartIter(parts.begin()-1, mainPart);}

	PartIter end() { return PartIter(parts.end(), mainPart); }
	ConstPartIter end() const { return ConstPartIter(parts.end(), mainPart);}

	const Part& operator[](size_t index) const { return (index == 0) ? *mainPart : *parts[index - 1].part; }
	Part& operator[](size_t index) { return (index == 0) ? *mainPart : *parts[index - 1].part; }
};
