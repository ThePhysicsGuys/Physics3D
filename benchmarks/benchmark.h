#pragma once

namespace P3D {
class Benchmark {
public:
	const char* name;
	Benchmark(const char* name);
	virtual ~Benchmark() {}
	virtual void init() {}
	virtual void run() = 0;
	virtual void printResults(double timeTaken) {}
};
};
