#pragma once

struct Physical;

#include "math/mat3.h"
#include "math/vec3.h"
#include "math/cframe.h"
#include "part.h"

typedef Vec3 Vec3Local;
typedef Vec3 Vec3Relative;

struct Physical {
	Part* part = nullptr;
	Vec3 velocity = Vec3();
	Vec3 angularVelocity = Vec3();

	Vec3 totalForce = Vec3();
	Vec3 totalMoment = Vec3();

	double mass;
	DiagonalMat3 inertia;

	Physical() = default;
	Physical(Part* part);
	inline Physical(Part* part, double mass, DiagonalMat3 inertia) : part(part), mass(mass), inertia(inertia) {};

	void update(double deltaT);
	void applyForceAtCenterOfMass(Vec3 force);
	void applyForce(Vec3Relative origin, Vec3 force);
	void applyMoment(Vec3 moment);

	Vec3 getCenterOfMass() const;
	Vec3 getAcceleration() const;
	Vec3 getAngularAcceleration() const;
	Vec3 getVelocityOfPoint(const Vec3Relative& point) const;
	Vec3 getAccelerationOfPoint(const Vec3Relative& point) const;
	SymmetricMat3 getPointAccelerationMatrix(const Vec3Local& localPoint) const;
	double getInertiaOfPointInDirection(const Vec3Local& localPoint, const Vec3Local& direction) const;
};
