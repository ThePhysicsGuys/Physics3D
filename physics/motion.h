#pragma once

#include "math/linalg/vec.h"
#include "math/rotation.h"

#include "math/taylorExpansion.h"

struct TranslationalMotion {
	Taylor<Vec3> translation;

	inline TranslationalMotion() : translation{Vec3(0.0, 0.0, 0.0), Vec3(0.0, 0.0, 0.0)} {}
	inline TranslationalMotion(Vec3 velocity) : translation{velocity, Vec3(0.0, 0.0, 0.0)} {}
	inline TranslationalMotion(Vec3 velocity, Vec3 acceleration) : translation{velocity, acceleration} {}
	inline TranslationalMotion(const Taylor<Vec3>& translation) : translation(translation) {}

	inline Vec3 getVelocity() const { return translation[0]; }
	inline Vec3 getAcceleration() const { return translation[1]; }

	inline Vec3 getOffsetAfterDeltaT(double deltaT) const {
		return translation(deltaT);
	}

	inline TranslationalMotion operator-() const {
		return TranslationalMotion{-translation};
	}

};

inline TranslationalMotion operator+(const TranslationalMotion& first, const TranslationalMotion& second) {
	return TranslationalMotion(first.translation + second.translation);
}
inline TranslationalMotion& operator+=(TranslationalMotion& first, const TranslationalMotion& second) {
	first.translation += second.translation;
	return first;
}

inline TranslationalMotion operator*(const TranslationalMotion& motion, double factor) {
	return TranslationalMotion(motion.translation * factor);
}
inline TranslationalMotion operator*(double factor, const TranslationalMotion& motion) {
	return motion * factor;
}


struct RotationalMotion {
	Taylor<Vec3> rotation;

	inline RotationalMotion() : rotation{Vec3(0.0, 0.0, 0.0), Vec3(0.0, 0.0, 0.0)} {}
	inline RotationalMotion(Vec3 angularVelocity) : rotation{angularVelocity, Vec3(0.0, 0.0, 0.0)} {}
	inline RotationalMotion(Vec3 angularVelocity, Vec3 angularAcceleration) : rotation{angularVelocity, angularAcceleration} {}
	inline RotationalMotion(const Taylor<Vec3>& rotation) : rotation(rotation) {}

	inline Vec3 getAngularVelocity() const { return rotation[0]; }
	inline Vec3 getAngularAcceleration() const { return rotation[1]; }

	inline Vec3 getVelocityOfPoint(Vec3 relativePoint) const {
		return getAngularVelocity() % relativePoint;
	}
	inline Vec3 getAccelerationOfPoint(Vec3 relativePoint) const {
		return getAngularAcceleration() % relativePoint + getAngularVelocity() % (getAngularVelocity() % relativePoint);
	}

	inline TranslationalMotion getTranslationalMotionOfPoint(Vec3 relativePoint) const {
		return TranslationalMotion(getVelocityOfPoint(relativePoint), getAccelerationOfPoint(relativePoint));
	}

	inline Vec3 getRotationAfterDeltaT(double deltaT) const {
		return rotation(deltaT);
	}
};

inline RotationalMotion operator*(const RotationalMotion& motion, double factor) {
	return RotationalMotion(motion.getAngularVelocity() * factor, motion.getAngularAcceleration() * factor);
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

	inline Motion(TranslationalMotion translationMotion) :
		translation(translationMotion),
		rotation() {}

	inline Motion(RotationalMotion rotationMotion) :
		translation(),
		rotation(rotationMotion) {}

	inline Motion(TranslationalMotion translationMotion, RotationalMotion rotationMotion) :
		translation(translationMotion),
		rotation(rotationMotion) {}

	inline Vec3 getVelocityOfPoint(Vec3 relativePoint) const {
		return translation.getVelocity() + rotation.getVelocityOfPoint(relativePoint);
	}
	inline Vec3 getAccelerationOfPoint(Vec3 relativePoint) const {
		return translation.getAcceleration() + rotation.getAccelerationOfPoint(relativePoint);
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
			translation.getVelocity()     + relativeMotion.translation.getVelocity(),
			rotation.getAngularVelocity()      + relativeMotion.rotation.getAngularVelocity(),
			translation.getAcceleration() + relativeMotion.translation.getAcceleration() + rotation.getAngularVelocity() % relativeMotion.translation.getVelocity() * 2,
			rotation.getAngularAcceleration() + relativeMotion.rotation.getAngularAcceleration() + rotation.getAngularVelocity() % relativeMotion.rotation.getAngularVelocity()
		);
	}

	inline Motion addOffsetRelativeMotion(Vec3 offset, const Motion& relativeMotion) const {
		return this->getMotionOfPoint(offset).addRelativeMotion(relativeMotion);
	}

	inline Movement getMovementAfterDeltaT(double deltaT) const {
		return Movement{translation.getOffsetAfterDeltaT(deltaT), rotation.getRotationAfterDeltaT(deltaT)};
	}

	inline Vec3 getVelocity() const { return translation.getVelocity(); }
	inline Vec3 getAcceleration() const { return translation.getAcceleration(); }
	inline Vec3 getAngularVelocity() const { return rotation.getAngularVelocity(); }
	inline Vec3 getAngularAcceleration() const { return rotation.getAngularAcceleration(); }
};

inline Motion operator+(const TranslationalMotion& motionOfStart, const Motion& motionToTranslate) {
	return Motion(motionOfStart + motionToTranslate.translation, motionToTranslate.rotation);
}

inline TranslationalMotion localToGlobal(const Rotation& rot, const TranslationalMotion& motion) {
	TranslationalMotion result;

	for(std::size_t i = 0; i < motion.translation.size(); i++) {
		result.translation[i] = rot.localToGlobal(motion.translation[i]);
	}

	return result;
}
inline TranslationalMotion globalToLocal(const Rotation& rot, const TranslationalMotion& motion) {
	TranslationalMotion result;

	for(std::size_t i = 0; i < motion.translation.size(); i++) {
		result.translation[i] = rot.globalToLocal(motion.translation[i]);
	}

	return result;
}
inline RotationalMotion localToGlobal(const Rotation& rot, const RotationalMotion& motion) {
	RotationalMotion result;

	for(std::size_t i = 0; i < motion.rotation.size(); i++) {
		result.rotation[i] = rot.localToGlobal(motion.rotation[i]);
	}

	return result;
}
inline RotationalMotion globalToLocal(const Rotation& rot, const RotationalMotion& motion) {
	RotationalMotion result;

	for(std::size_t i = 0; i < motion.rotation.size(); i++) {
		result.rotation[i] = rot.globalToLocal(motion.rotation[i]);
	}

	return result;
}

inline Motion localToGlobal(const Rotation& rot, const Motion& motion) {
	return Motion(localToGlobal(rot, motion.translation),
				  localToGlobal(rot, motion.rotation));
}

inline Motion globalToLocal(const Rotation& rot, const Motion& motion) {
	return Motion(globalToLocal(rot, motion.translation),
				  globalToLocal(rot, motion.rotation));
}
