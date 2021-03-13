#include "benchmark.h"

#include <iostream>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

#include "../physics/threading/threadPool.h"

using namespace std::chrono;

class ThreadCreateBenchmark : public Benchmark {
public:
	ThreadCreateBenchmark() : Benchmark("threadCreateResponseTime") {}

	virtual void init() override {}

	virtual void run() override {
		decltype(high_resolution_clock::now()) start;

		std::mutex coutMutex;

		std::cout << "\n";
		auto work = [&start,&coutMutex]() {
			auto response = high_resolution_clock::now();

			nanoseconds delay = response - start;

			coutMutex.lock();
			std::cout << delay.count() / 1000 << " microseconds\n";
			coutMutex.unlock();
			std::this_thread::sleep_for(milliseconds(1000));
		};

		std::vector<std::thread> threads(std::thread::hardware_concurrency()-1);

		for(int iter = 0; iter < 5; iter++) {
			std::cout << "Run " << iter << "\n";
			start = high_resolution_clock::now();
			for(std::thread& t : threads) t = std::thread(work);
			work();
			for(std::thread& t : threads) t.join();
		}
	}

	virtual void printResults(double timeTaken) override {}

} threadCreate;

class ThreadPoolBenchmark : public Benchmark {
public:
	ThreadPoolBenchmark() : Benchmark("threadPoolResponseTime") {}

	virtual void init() override {}

	virtual void run() override {
		decltype(high_resolution_clock::now()) start;

		std::mutex coutMutex;

		std::cout << "\n";
		auto work = [&start, &coutMutex]() {
			auto response = high_resolution_clock::now();

			nanoseconds delay = response - start;

			coutMutex.lock();
			std::cout << delay.count() / 1000 << " microseconds\n";
			coutMutex.unlock();
			std::this_thread::sleep_for(milliseconds(1000));
		};
		
		std::vector<std::thread> threads(std::thread::hardware_concurrency() - 1);

		ThreadPool threadPool;

		for(int iter = 0; iter < 5; iter++) {
			std::cout << "Run " << iter << "\n";
			start = high_resolution_clock::now();
			threadPool.doInParallel(work);
		}
	}

	virtual void printResults(double timeTaken) override {}

} threadPool;

