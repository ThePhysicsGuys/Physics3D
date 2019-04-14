#pragma once

struct Physical;

#include "math/mat3.h"
#include "math/vec3.h"
#include "math/cframe.h"
#include "part.h"

typedef Vec3 Vec3Local;
typedef Vec3 Vec3Relative;

struct Physical {
	Part part;
	Vec3 velocity = Vec3();
	Vec3 angularVelocity = Vec3();

	Vec3 totalForce = Vec3();
	Vec3 totalMoment = Vec3();

	double mass;
	Mat3 inertia;

	Physical() = default;
	Physical(Part part);

	void update(double deltaT);
	void applyForceAtCenterOfMass(Vec3 force);
	void applyForce(Vec3Relative origin, Vec3 force);
	void applyMoment(Vec3 moment);

	Vec3 getCenterOfMass() const;
	Vec3 getVelocityOfPoint(Vec3Relative point) const;
};
