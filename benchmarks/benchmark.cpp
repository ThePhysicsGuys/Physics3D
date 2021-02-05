#include "benchmark.h"

#include <chrono>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include "../util/terminalColor.h"
#include "../util/parseCPUIDArgs.h"

std::vector<Benchmark*>* knownBenchmarks = nullptr;

Benchmark::Benchmark(const char* name) : name(name) {
	if(knownBenchmarks == nullptr) { knownBenchmarks = new std::vector<Benchmark*>(); }
	knownBenchmarks->push_back(this);
}

Benchmark* getBenchFor(std::string id) {
	if(id.length() == 0) return nullptr;
	if(id[0] >= '0' && id[0] <= '9') {
		return (*knownBenchmarks)[std::stoi(id)];
	} else {
		for(Benchmark* b : *knownBenchmarks) {
			if(id == b->name) {
				return b;
			}
		}
	}
	return nullptr;
}

std::vector<std::string> split(std::string str, char splitChar) {
	std::vector<std::string> substrings;

	int lastIndex = 0;
	int index = 0;
	while(index < str.length()){
		while(str[index] != splitChar) {
			index++;
		}
		substrings.push_back(str.substr(lastIndex, index - lastIndex));
		index++;
		lastIndex = index;
	}
	return substrings;
}

static void runBenchmark(Benchmark* bench) {
	setColor(TerminalColor::CYAN);
	
	auto createStart = std::chrono::high_resolution_clock::now();
	bench->init();
	auto createFinish = std::chrono::high_resolution_clock::now();
	std::cout << bench->name << ": ";
	std::cout.flush();
	setColor(TerminalColor::YELLOW);
	std::cout << '(' << (createFinish - createStart).count() / 1000000.0 << "ms)";
	std::cout.flush();
	auto runStart = std::chrono::high_resolution_clock::now();
	bench->run();
	auto runFinish = std::chrono::high_resolution_clock::now();
	double deltaTimeMS = (runFinish - runStart).count() / 1000000.0;
	setColor(TerminalColor::GREEN);
	std::cout << "  (" << deltaTimeMS << "ms)\n";
	std::cout.flush();
	bench->printResults(deltaTimeMS);
}

static void runBenchmarks(const std::vector<std::string>& benchmarks) {
	setColor(TerminalColor::CYAN);
	std::cout << "[NAME]";
	setColor(TerminalColor::YELLOW);
	std::cout << " [INITIALIZATION]";
	setColor(TerminalColor::GREEN);
	std::cout << " [RUNTIME]\n";
	setColor(TerminalColor::WHITE);

	for(const std::string& c : benchmarks) {
		Benchmark* b = getBenchFor(c);
		if(b != nullptr) {
			runBenchmark(b);
		}
	}
}

int main(int argc, const char** args) {
	Util::ParsedArgs pa(argc, args);
	std::cout << Util::printAndParseCPUIDArgs(pa).c_str() << "\n";

	if(pa.argCount() >= 1) {
		runBenchmarks(pa.args());
	} else {
		setColor(TerminalColor::WHITE);
		std::cout << "The following benchmarks are available:\n";
		setColor(TerminalColor::CYAN);

		for(std::size_t i = 0; i < knownBenchmarks->size(); i++) {
			std::cout << i << ") " << (*knownBenchmarks)[i]->name << "\n";
		}

		setColor(TerminalColor::WHITE);
		std::cout << "Run> ";
		setColor(TerminalColor::GREEN);
		std::string cmd;
		std::cin >> cmd;
		if(cmd.empty()) {
			setColor(TerminalColor::WHITE);
			return 0;
		}
		cmd.append(";");

		std::vector<std::string> commands = split(cmd, ';');

		runBenchmarks(commands);
	}
	return 0;
}
