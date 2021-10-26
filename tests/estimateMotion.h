#pragma once

#include <Physics3D/motion.h>
#include <Physics3D/math/linalg/vec.h>
#include <Physics3D/math/position.h>
#include <Physics3D/math/globalCFrame.h>

#include <utility>

namespace P3D {
Vec3 getVelocityBySimulation(const Motion& m, const Vec3& point, double deltaT);
Motion getMotionBySimulation(const Motion& m, const Vec3& point, double deltaT);
TranslationalMotion estimateMotion(const Vec3& startPos, const Vec3& midPos, const Vec3& endPos, double stepT);
TranslationalMotion estimateMotion(const Position& startPos, const Position& midPos, const Position& endPos, double stepT);
Vec3 getRotationVelFor(const Rotation& before, const Rotation& after, double deltaT);
Motion estimateMotion(const CFrame& step1, const CFrame& step2, const CFrame& step3, double stepT);
Motion estimateMotion(const GlobalCFrame& step1, const GlobalCFrame& step2, const GlobalCFrame& step3, double stepT);

CFrame simulateForTime(const Motion& motion, const CFrame& startingCFrame, double deltaT);
};

