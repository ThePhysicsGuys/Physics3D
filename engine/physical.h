#pragma once

struct Physical;

#include "math/mat3.h"
#include "math/vec3.h"
#include "math/cframe.h"
#include "part.h"

#include <vector>

typedef Vec3 Vec3Local;
typedef Vec3 Vec3Relative;

struct AttachedPart {
	CFrame attachment;
	Part* part;
};

struct PartIter {
	std::_Vector_iterator<std::_Vector_val<std::_Simple_types<AttachedPart>>> iter;

	inline Part& operator*() const {
		AttachedPart& atPart = *iter;
		return *(atPart.part);
	}
	inline void operator++() {iter++;}
	inline bool operator!=(const PartIter& other) const {return this->iter != other.iter;}
	inline bool operator==(const PartIter& other) const { return this->iter == other.iter; }
};
struct ConstPartIter {
	std::_Vector_const_iterator<std::_Vector_val<std::_Simple_types<AttachedPart>>> iter;

	inline const Part& operator*() const { return *(*iter).part; }
	inline void operator++() { iter++; }
	inline bool operator!=(const ConstPartIter& other) const { return this->iter != other.iter; }
	inline bool operator==(const ConstPartIter& other) const { return this->iter == other.iter; }
};

struct Physical {
	CFrame cframe;
	double maxRadius;
	std::vector<AttachedPart> parts;
	Vec3 velocity = Vec3();
	Vec3 angularVelocity = Vec3();

	Vec3 totalForce = Vec3();
	Vec3 totalMoment = Vec3();

	double mass;
	Vec3 centerOfMass;
	SymmetricMat3 inertia;

	Physical() = default;
	Physical(Part* part);
	inline Physical(Part* part, double mass, SymmetricMat3 inertia) : cframe(part->cframe), mass(mass), inertia(inertia) { parts.push_back(AttachedPart{ CFrame(), part }); };

	Physical(Physical&& other) noexcept {
		this->cframe = other.cframe;
		this->maxRadius = other.maxRadius;
		this->parts = std::move(other.parts);
		this->velocity = other.velocity;
		this->angularVelocity = other.angularVelocity;
		this->totalForce = other.totalForce;
		this->totalMoment = other.totalMoment;
		this->mass = other.mass;
		this->centerOfMass = other.centerOfMass;
		this->inertia = other.inertia;

		for (AttachedPart& p : parts) {
			p.part->parent = this;
		}
	}

	Physical& operator=(Physical&& other) noexcept {
		this->cframe = other.cframe;
		this->maxRadius = other.maxRadius;
		this->parts = std::move(other.parts);
		this->velocity = other.velocity;
		this->angularVelocity = other.angularVelocity;
		this->totalForce = other.totalForce;
		this->totalMoment = other.totalMoment;
		this->mass = other.mass;
		this->centerOfMass = other.centerOfMass;
		this->inertia = other.inertia;

		for (AttachedPart& p : parts) {
			p.part->parent = this;
		}
		return *this;
	}

	Physical(const Physical&) = delete;
	void operator=(const Physical&) = delete;

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

	void setCFrame(const CFrame& newCFrame);
	
	inline CFrame& getCFrame() { return cframe; }

	Vec3 getCenterOfMass() const;
	Vec3 getAcceleration() const;
	Vec3 getAngularAcceleration() const;
	Vec3 getVelocityOfPoint(const Vec3Relative& point) const;
	Vec3 getAccelerationOfPoint(const Vec3Relative& point) const;
	SymmetricMat3 getPointAccelerationMatrix(const Vec3Local& localPoint) const;
	double getInertiaOfPointInDirection(const Vec3Local& localPoint, const Vec3Local& direction) const;
	double getVelocityKineticEnergy() const;
	double getAngularKineticEnergy() const;
	double getKineticEnergy() const;
	size_t getPartCount() const { return parts.size(); }

	PartIter begin() { return PartIter{ parts.begin() }; }
	ConstPartIter begin() const { return ConstPartIter{ parts.begin() }; }

	PartIter end() { return PartIter{ parts.end() }; }
	ConstPartIter end() const { return ConstPartIter{ parts.end() }; }
};
