#pragma once

#include "../physics/motion.h"
#include "../physics/math/linalg/vec.h"
#include "../physics/math/position.h"
#include "../physics/math/globalCFrame.h"

#include <utility>

Vec3 getVelocityBySimulation(const Motion& m, const Vec3& point, double deltaT);
Motion getMotionBySimulation(const Motion& m, const Vec3& point, double deltaT);
TranslationalMotion estimateMotion(const Vec3& startPos, const Vec3& midPos, const Vec3& endPos, double stepT);
TranslationalMotion estimateMotion(const Position& startPos, const Position& midPos, const Position& endPos, double stepT);
Vec3 getRotationVelFor(const Rotation& before, const Rotation& after, double deltaT);
Motion estimateMotion(const CFrame& step1, const CFrame& step2, const CFrame& step3, double stepT);
Motion estimateMotion(const GlobalCFrame& step1, const GlobalCFrame& step2, const GlobalCFrame& step3, double stepT);

CFrame simulateForTime(const Motion& motion, const CFrame& startingCFrame, double deltaT);

