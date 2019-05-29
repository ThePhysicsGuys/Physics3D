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

struct Physical {
	CFrame cframe;
	double maxRadius;
	std::vector<AttachedPart> parts;
	Vec3 velocity = Vec3();
	Vec3 angularVelocity = Vec3();

	Vec3 totalForce = Vec3();
	Vec3 totalMoment = Vec3();

	double mass;
	DiagonalMat3 inertia;

	Physical() = default;
	Physical(Part* part);
	inline Physical(Part* part, double mass, DiagonalMat3 inertia) : cframe(part->cframe), mass(mass), inertia(inertia) { parts.push_back(AttachedPart{ CFrame(), part }); };

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
};
