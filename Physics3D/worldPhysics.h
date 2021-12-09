#pragma once

#include "part.h"
#include "math/linalg/vec.h"
#include "math/position.h"
#include "colissionBuffer.h"
#include "world.h"
#include "threading/threadPool.h"
#include "threading/upgradeableMutex.h"

namespace P3D {
void handleCollision(Part& part1, Part& part2, Position collisionPoint, Vec3 exitVector);
void handleTerrainCollision(Part& part1, Part& part2, Position collisionPoint, Vec3 exitVector);
PartIntersection safeIntersects(const Part& p1, const Part& p2);
void refineColissions(std::vector<Colission>& colissions);
void parallelRefineColissions(ThreadPool& threadPool, std::vector<Colission>& colissions);
void findColissions(WorldPrototype& world, ColissionBuffer& curColissions);
void findColissionsParallel(WorldPrototype& world, ColissionBuffer& curColissions, ThreadPool& threadPool);
void applyExternalForces(WorldPrototype& world);
void handleColissions(ColissionBuffer& curColissions);
void handleConstraints(WorldPrototype& world);
void update(WorldPrototype& world);

void tickWorldUnsynchronized(WorldPrototype& world, ThreadPool& threadPool);
void tickWorldSynchronized(WorldPrototype& world, ThreadPool& threadPool, UpgradeableMutex& worldMutex);
};

