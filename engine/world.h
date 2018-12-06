#pragma once

#include "part.h"
#include "physical.h"
#include <vector>
#include <mutex>

class World {
public:
	std::vector<Physical> physicals;
	std::mutex lock;

	World();

	World(const World&) = delete;
	World(World&&) = delete;
	World& operator=(const World&) = delete;
	World& operator=(World&&) = delete;

	void tick(double deltaT);
	void addObject(Physical p);


};
