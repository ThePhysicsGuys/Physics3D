#include "benchmark.h"

#include <chrono>
#include <vector>
#include <iostream>
#include <string>

#include "../util/terminalColor.h"

std::vector<Benchmark*>* knownBenchmarks = nullptr;

Benchmark::Benchmark(const char* name) : name(name) {
	if(knownBenchmarks == nullptr) { knownBenchmarks = new std::vector<Benchmark*>(); }
	knownBenchmarks->push_back(this);
}

int main() {
	std::cout << "The following benchmarks are available:\n";
	setColor(TerminalColor::CYAN);
	for(Benchmark* b : *knownBenchmarks) {
		std::cout << "  " << b->name << "\n";
	}

	Benchmark* bench;
	while(true) {
		setColor(TerminalColor::WHITE);
		std::cout << "Run> ";
		setColor(TerminalColor::GREEN);
		std::string benchName;
		std::cin >> benchName;
		for(Benchmark* b : *knownBenchmarks) {
			if(benchName == b->name) {
				bench = b;
				goto found;
			}
		}
	}
	found:

	setColor(TerminalColor::WHITE);
	
	std::cout << "creating benchmark\n";
	auto createStart = std::chrono::high_resolution_clock::now();
	bench->init();
	auto createFinish = std::chrono::high_resolution_clock::now();
	std::cout << "finished creating benchmark, took " << (createFinish - createStart).count() / 1000000.0 << "ms\n";
	std::cout << "running...\n";
	auto runStart = std::chrono::high_resolution_clock::now();
	bench->run();
	auto runFinish = std::chrono::high_resolution_clock::now();
	double deltaTimeMS = (runFinish - runStart).count() / 1000000.0;
	std::cout << "finished after " << deltaTimeMS << "ms\n";
	bench->printResults(deltaTimeMS);
	setColor(TerminalColor::WHITE);
	return 0;
}
