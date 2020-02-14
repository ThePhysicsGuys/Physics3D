#pragma once

#include "math/linalg/vec.h"
#include "math/cframe.h"

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
	inline Motion() : velocity(0,0,0), angularVelocity(0,0,0), acceleration(0,0,0), angularAcceleration(0,0,0) {};

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

	inline Motion getMotionOfPoint(Vec3 relativePoint) const {
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
			acceleration + relativeMotion.acceleration + this->angularVelocity % relativeMotion.velocity * 2,
			angularAcceleration + relativeMotion.angularAcceleration + angularVelocity % relativeMotion.angularVelocity
		);
	}

	inline Motion addOffsetRelativeMotion(Vec3 offset, const Motion& relativeMotion) const {
		return this->getMotionOfPoint(offset).addRelativeMotion(relativeMotion);
	}


	struct Movement {
		Vec3 translation;
		Vec3 rotation;
	};

	inline Movement getMovementAfterDeltaT(double deltaT) const {
		Vec3 deltaPos = velocity * deltaT + acceleration * deltaT * deltaT / 2;

		Vec3 rotation = angularVelocity * deltaT + angularAcceleration * deltaT * deltaT / 2;

		return Movement{deltaPos, rotation};
	}
};

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