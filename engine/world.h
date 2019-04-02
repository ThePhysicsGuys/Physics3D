#pragma once

#include "part.h"
#include "physical.h"
#include <vector>
#include <queue>
#include <mutex>
#include <shared_mutex>

class World {
private:
	size_t getTotalVertexCount();
	void processQueue();

public:
	std::vector<Physical> physicals;
	std::queue<Physical> newPhysicalQueue;
	mutable std::shared_mutex lock;
	mutable std::mutex queueLock;

	Physical* selectedPhysical = nullptr;
	Vec3 localSelectedPoint;
	Vec3 magnetPoint;

	World();

	World(const World&) = delete;
	World(World&&) = delete;
	World& operator=(const World&) = delete;
	World& operator=(World&&) = delete;

	void tick(double deltaT);
	void addObject(Physical& p);
	Physical& addObject(Shape s, CFrame location, double density, double friction);
	Physical& addObject(Part& p);

	virtual void applyExternalForces(const Shape* transformedShapes);
};
