#pragma once

#include "../physics/motion.h"
#include "../physics/math/linalg/vec.h"
#include "../physics/math/position.h"
#include "../physics/math/globalCFrame.h"

#include <utility>

Vec3 getVelocityBySimulation(const Motion& m, const Vec3& point, double deltaT);
Motion getMotionBySimulation(const Motion& m, const Vec3& point, double deltaT);
std::pair<Vec3, Vec3> estimateMotion(const Position& startPos, const Position& midPos, const Position& endPos, double stepT);
Vec3 getRotationVelFor(const RotMat3& before, const RotMat3& after, double deltaT);
Motion estimateMotion(const GlobalCFrame& step1, const GlobalCFrame& step2, const GlobalCFrame& step3, double stepT);
