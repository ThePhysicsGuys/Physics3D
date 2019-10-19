#include "benchmark.h"
#include "benchmarks.h"

#include <chrono>

#include "../util/log.h"


int main() {
	Log::print("creating benchmark\n");
	auto createStart = std::chrono::high_resolution_clock::now();
	Benchmark* b = basicWorld();
	auto createFinish = std::chrono::high_resolution_clock::now();
	Log::print("finished creating benchmark, took %fms\n", (createFinish - createStart).count() / 1000000.0);
	Log::print("running...\n");
	auto runStart = std::chrono::high_resolution_clock::now();
	b->run();
	auto runFinish = std::chrono::high_resolution_clock::now();
	double deltaTimeMS = (runFinish - runStart).count() / 1000000.0;
	Log::print("finished after %fms\n", deltaTimeMS);
	b->printResults(deltaTimeMS);
	Log::setColor(Log::WHITE);
}
