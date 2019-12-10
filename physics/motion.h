#pragma once

#include "math/linalg/vec.h"

struct Motion {
	union {
		struct {
			Vec3 velocity;
			Vec3 angularVelocity;
			Vec3 acceleration;
			Vec3 angularAcceleration;
		};
		Vec3 components[4];
	};
	inline Motion() : components{} {};

	inline Motion(Vec3 velocity, Vec3 angularVelocity) : 
		velocity(velocity), 
		angularVelocity(angularVelocity),
		acceleration(0.0, 0.0, 0.0),
		angularAcceleration(0.0, 0.0, 0.0) {};
	inline Motion(Vec3 velocity, Vec3 angularVelocity, Vec3 acceleration, Vec3 angularAcceleration) : 
		velocity(velocity), 
		angularVelocity(angularVelocity),
		acceleration(acceleration), 
		angularAcceleration(angularAcceleration) {};

	inline Vec3 getVelocityOfPoint(Vec3 relativePoint) const {
		return velocity + angularVelocity % relativePoint;
	}
	inline Vec3 getAccelerationOfPoint(Vec3 relativePoint) const {
		return acceleration + angularAcceleration % relativePoint + angularVelocity % (angularVelocity % relativePoint);
	}

	inline Motion getMotionOfRelativePoint(Vec3 relativePoint) const {
		return Motion(
			getVelocityOfPoint(relativePoint), 
			angularVelocity, 
			getAccelerationOfPoint(relativePoint), 
			angularAcceleration
		);
	}

	inline Motion addRelativeMotion(const Motion& relativeMotion) const {
		return Motion(
			velocity + relativeMotion.velocity,
			angularVelocity + relativeMotion.angularVelocity,
			acceleration + relativeMotion.acceleration,
			angularAcceleration + relativeMotion.angularAcceleration + angularVelocity % relativeMotion.angularVelocity
		);
	}
};

inline Motion localToGlobal(const CFrame& cframe, const Motion& motion) {
	
}

inline Motion globalToLocal(const CFrame& cframe, const Motion& motion) {
	
}

inline Motion localToRelative(const CFrame& cframe, const Motion& motion) {
	Motion result;
	for(int i = 0; i < 4; i++) {
		result.components[i] = cframe.localToRelative(motion.components[i]);
	}
	return result;
}

inline Motion relativeToLocal(const CFrame& cframe, const Motion& motion) {
	Motion result;
	for(int i = 0; i < 4; i++) {
		result.components[i] = cframe.relativeToLocal(motion.components[i]);
	}
	return result;
}