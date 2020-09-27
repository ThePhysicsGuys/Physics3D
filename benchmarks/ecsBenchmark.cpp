#include "benchmark.h"
#include "../engine/ecs/registry.h"
#include "../util/log.h"

class ECSGetFromRegistryBenchmark : public Benchmark {
public:
	ECSGetFromRegistryBenchmark() : Benchmark("ecsGetFromRegistryBenchmark") {}

	P3D::Engine::Registry64 registry;
	std::vector<int> v { 1, 2, 3, 4, 5, 6 };
	int errors = 0;
	
	struct A {
		int i; 
		
		A(int i) : i(i) {}
	};

	void init() override {
		int amount = 1000000;
		for (int i = 0; i < amount; i++) {
			auto id = registry.create();
			registry.add<A>(id, i);
		}
	}

	void run() override {
		int i = 0;
		auto view = registry.view<A>();
		for (auto entity : view) {
			auto& comp = *registry.get<A>(entity);
			if (comp.i != i)
				errors++;
			i++;
		}
	}

	void printResults(double timeTaken) override {
		Log::error("Amount of errors: %d\n", errors);
	}

} ecsGetFromRegistryBenchmark;

class ECSGetFromViewBenchmark : public Benchmark {
public:
	ECSGetFromViewBenchmark() : Benchmark("ecsGetFromViewBenchmark") {}

	P3D::Engine::Registry64 registry;
	int errors = 0;

	struct A { int i; A(int i) : i(i) {} };

	void init() override {
		int amount = 1000000;
		for (int i = 0; i < amount; i++) {
			auto id = registry.create();
			registry.add<A>(id, i);
		}
	}

	void run() override {
		int i = 0;
		auto view = registry.view<A>();
		for (auto entity : view) {
			auto& comp = view.get<A>(entity);
			if (comp.i != i)
				errors++;
			i++;
		}
	}

	void printResults(double timeTaken) override {
		Log::error("Amount of errors: %d\n", errors);
	}

} ecsGetFromViewBenchmark;