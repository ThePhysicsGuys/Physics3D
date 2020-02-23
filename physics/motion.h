#pragma once

#include "math/linalg/vec.h"
#include "math/rotation.h"

struct TranslationalMotion {
	Vec3 velocity;
	Vec3 acceleration;

	inline TranslationalMotion() : velocity(0.0, 0.0, 0.0), acceleration(0.0, 0.0, 0.0) {}
	inline TranslationalMotion(Vec3 velocity) : velocity(velocity), acceleration(0.0, 0.0, 0.0) {}
	inline TranslationalMotion(Vec3 velocity, Vec3 acceleration) : velocity(velocity), acceleration(acceleration) {}

	inline Vec3 getOffsetAfterDeltaT(double deltaT) const {
		return velocity * deltaT + acceleration * deltaT * deltaT / 2;
	}

	inline TranslationalMotion operator-() const {
		return TranslationalMotion(-velocity, -acceleration);
	}
};

inline TranslationalMotion operator+(const TranslationalMotion& first, const TranslationalMotion& second) {
	return TranslationalMotion(first.velocity + second.velocity, first.acceleration + second.acceleration);
}
inline TranslationalMotion& operator+=(TranslationalMotion& first, const TranslationalMotion& second) {
	first.velocity += second.velocity;
	first.acceleration += second.acceleration;
	return first;
}

inline TranslationalMotion operator*(const TranslationalMotion& motion, double factor) {
	return TranslationalMotion(motion.velocity * factor, motion.acceleration * factor);
}
inline TranslationalMotion operator*(double factor, const TranslationalMotion& motion) {
	return motion * factor;
}


struct RotationalMotion {
	Vec3 angularVelocity;
	Vec3 angularAcceleration;

	inline RotationalMotion() : angularVelocity(0.0,0.0,0.0), angularAcceleration(0.0,0.0,0.0) {}
	inline RotationalMotion(Vec3 angularVelocity) : angularVelocity(angularVelocity), angularAcceleration(0.0, 0.0, 0.0) {}
	inline RotationalMotion(Vec3 angularVelocity, Vec3 angularAcceleration) : angularVelocity(angularVelocity), angularAcceleration(angularAcceleration) {}

	inline Vec3 getVelocityOfPoint(Vec3 relativePoint) const {
		return angularVelocity % relativePoint;
	}
	inline Vec3 getAccelerationOfPoint(Vec3 relativePoint) const {
		return angularAcceleration % relativePoint + angularVelocity % (angularVelocity % relativePoint);
	}

	inline TranslationalMotion getTranslationalMotionOfPoint(Vec3 relativePoint) const {
		return TranslationalMotion(getVelocityOfPoint(relativePoint), getAccelerationOfPoint(relativePoint));
	}

	inline Vec3 getRotationAfterDeltaT(double deltaT) const {
		return angularVelocity * deltaT + angularAcceleration * deltaT * deltaT / 2;
	}
};

inline RotationalMotion operator*(const RotationalMotion& motion, double factor) {
	return RotationalMotion(motion.angularVelocity * factor, motion.angularAcceleration * factor);
}
inline RotationalMotion operator*(double factor, const RotationalMotion& motion) {
	return motion * factor;
}

struct Movement {
	Vec3 translation;
	Vec3 rotation;
};
struct Motion {
	TranslationalMotion translation;
	RotationalMotion rotation;

	inline Motion() = default;

	inline Motion(Vec3 velocity, Vec3 angularVelocity) :
		translation(velocity),
		rotation(angularVelocity) {}
	inline Motion(Vec3 velocity, Vec3 angularVelocity, Vec3 acceleration, Vec3 angularAcceleration) :
		translation(velocity, acceleration),
		rotation(angularVelocity, angularAcceleration) {}

	inline Motion(TranslationalMotion translationMotion, RotationalMotion rotationMotion) :
		translation(translationMotion),
		rotation(rotationMotion) {}

	inline Vec3 getVelocityOfPoint(Vec3 relativePoint) const {
		return translation.velocity + rotation.getVelocityOfPoint(relativePoint);
	}
	inline Vec3 getAccelerationOfPoint(Vec3 relativePoint) const {
		return translation.acceleration + rotation.getAccelerationOfPoint(relativePoint);
	}
	inline TranslationalMotion getTranslationalMotionOfPoint(Vec3 relativePoint) const {
		return translation + rotation.getTranslationalMotionOfPoint(relativePoint);
	}

	inline Motion getMotionOfPoint(Vec3 relativePoint) const {
		return Motion(
			getTranslationalMotionOfPoint(relativePoint),
			rotation
		);
	}

	inline Motion addRelativeMotion(const Motion& relativeMotion) const {
		return Motion(
			translation.velocity         + relativeMotion.translation.velocity,
			rotation.angularVelocity     + relativeMotion.rotation.angularVelocity,
			translation.acceleration     + relativeMotion.translation.acceleration     + rotation.angularVelocity % relativeMotion.translation.velocity * 2,
			rotation.angularAcceleration + relativeMotion.rotation.angularAcceleration + rotation.angularVelocity % relativeMotion.rotation.angularVelocity
		);
	}

	inline Motion addOffsetRelativeMotion(Vec3 offset, const Motion& relativeMotion) const {
		return this->getMotionOfPoint(offset).addRelativeMotion(relativeMotion);
	}

	inline Movement getMovementAfterDeltaT(double deltaT) const {
		return Movement{translation.getOffsetAfterDeltaT(deltaT), rotation.getRotationAfterDeltaT(deltaT)};
	}
};

inline Motion operator+(const TranslationalMotion& motionOfStart, const Motion& motionToTranslate) {
	return Motion(motionOfStart + motionToTranslate.translation, motionToTranslate.rotation);
}

inline TranslationalMotion localToGlobal(const Rotation& rot, const TranslationalMotion& motion) {
	return TranslationalMotion(rot.localToGlobal(motion.velocity), rot.localToGlobal(motion.acceleration));
}
inline TranslationalMotion globalToLocal(const Rotation& rot, const TranslationalMotion& motion) {
	return TranslationalMotion(rot.globalToLocal(motion.velocity), rot.globalToLocal(motion.acceleration));
}
inline RotationalMotion localToGlobal(const Rotation& rot, const RotationalMotion& motion) {
	return RotationalMotion(rot.localToGlobal(motion.angularVelocity), rot.localToGlobal(motion.angularAcceleration));
}
inline RotationalMotion globalToLocal(const Rotation& rot, const RotationalMotion& motion) {
	return RotationalMotion(rot.globalToLocal(motion.angularVelocity), rot.globalToLocal(motion.angularAcceleration));
}

inline Motion localToGlobal(const Rotation& rot, const Motion& motion) {
	return Motion(localToGlobal(rot, motion.translation),
				  localToGlobal(rot, motion.rotation));
}

inline Motion globalToLocal(const Rotation& rot, const Motion& motion) {
	return Motion(globalToLocal(rot, motion.translation),
				  globalToLocal(rot, motion.rotation));
}
