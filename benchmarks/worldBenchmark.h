#pragma once

#include "benchmark.h"
#include "../physics/world.h"

class WorldBenchmark : public Benchmark {
protected:
	World<Part> world;
	int tickCount;

public:
	WorldBenchmark(const char* name, int tickCount);

	virtual void run() override;
	virtual void printResults(double timeTaken) override;
};
