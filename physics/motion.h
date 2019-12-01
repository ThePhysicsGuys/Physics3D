#pragma once

#include "math/linalg/vec.h"

struct Motion {
	Vec3 velocity;
	Vec3 angularVelocity;
	Vec3 acceleration = Vec3(0.0, 0.0, 0.0);
	Vec3 angularAcceleration = Vec3(0.0, 0.0, 0.0);

	inline Motion(Vec3 velocity, Vec3 angularVelocity) : 
		velocity(velocity), 
		angularVelocity(angularVelocity) {};
	inline Motion(Vec3 velocity, Vec3 angularVelocity, Vec3 acceleration, Vec3 angularAcceleration) : 
		velocity(velocity), 
		angularVelocity(angularVelocity),
		acceleration(acceleration), 
		angularAcceleration(angularAcceleration) {};

	inline Vec3 getVelocityOfPoint(Vec3 relativePoint) const {
		return velocity + angularVelocity % relativePoint;
	}
	inline Vec3 getAccelerationOfPoint(Vec3 relativePoint) const {
		acceleration + angularAcceleration % relativePoint + angularVelocity % (angularVelocity % relativePoint);
	}
};
