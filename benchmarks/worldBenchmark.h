#pragma once

#include "benchmark.h"
#include "../physics/world.h"

static const PartProperties basicProperties{1.0, 0.7, 0.5};
class WorldBenchmark : public Benchmark {
protected:
	World<Part> world;
	int tickCount;

public:
	WorldBenchmark(const char* name, int tickCount);

	virtual void run() override;
	virtual void printResults(double timeTaken) override;

	void createFloor(double w, double h, double wallHeight);
};
