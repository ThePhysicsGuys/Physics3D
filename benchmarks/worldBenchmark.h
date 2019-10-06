#pragma once

#include "benchmark.h"
#include "../engine/world.h"

class WorldBenchmark : public Benchmark {
protected:
	World<Part> world;
	int tickCount;

public:
	WorldBenchmark(int tickCount);

	virtual void run() override;
	virtual void printResults(double timeTaken) override;
};
