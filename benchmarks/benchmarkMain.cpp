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
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
