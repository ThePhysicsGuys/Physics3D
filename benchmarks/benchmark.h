#pragma once


class Benchmark {
public:
	virtual ~Benchmark() {}
	virtual void run() = 0;
	virtual void printResults(double timeTaken) {}
};
