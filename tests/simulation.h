#pragma once


#include "../physics/math/linalg/vec.h"
#include "../physics/math/taylorExpansion.h"
#include "../physics/math/rotation.h"
#include "../physics/math/cframe.h"
#include "../physics/motion.h"

#include <array>


template<std::size_t Size>
std::array<Vec3, Size + 1> computeTranslationOverTime(Vec3 start, TaylorExpansion<Vec3, Size> derivatives, double deltaT) {
	std::array<Vec3, Size + 1> result;

	for(std::size_t i = 0; i < Size+1; i++) {
		result[i] = start + derivatives(deltaT * i);
	}

	return result;
}

template<std::size_t Size>
std::array<Rotation, Size + 1> computeRotationOverTime(Rotation start, TaylorExpansion<Vec3, Size> derivatives, double deltaT) {
	std::array<Rotation, Size + 1> result;

	for(std::size_t i = 0; i < Size + 1; i++) {
		result[i] = Rotation::fromRotationVec(derivatives(deltaT * i)) * start;
	}

	return result;
}

inline std::array<Vec3, 3> computeTranslationOverTime(Vec3 start, TranslationalMotion motion, double deltaT) {
	return computeTranslationOverTime(start, motion.translation, deltaT);
}
inline std::array<Rotation, 3> computeRotationOverTime(Rotation start, RotationalMotion motion, double deltaT) {
	return computeRotationOverTime(start, motion.rotation, deltaT);
}
inline std::array<CFrame, 3> computeCFrameOverTime(CFrame start, Motion motion, double deltaT) {
	std::array<CFrame, 3> result;
	
	for(std::size_t i = 0; i < 3; i++) {
		double t = deltaT * i;
		Vec3 offset = start.getPosition() + motion.translation.getOffsetAfterDeltaT(t);
		Rotation rot = Rotation::fromRotationVec(motion.rotation.getRotationAfterDeltaT(t)) * start.getRotation();

		result[i] = CFrame(offset, rot);
	}

	return result;
}

template<typename T>
inline FullTaylorExpansion<T, T, 2> estimateDerivatives(const std::array<T, 3>& points, double deltaT) {
	FullTaylorExpansion<T, T, 2> result(points[0]);
	result.derivatives[0] = (points[1] - points[0]) / deltaT;
	result.derivatives[1] = (points[0] + points[2] - points[1] * 2.0) / (deltaT * deltaT);

	return result;
}
