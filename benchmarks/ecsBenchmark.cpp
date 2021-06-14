#include "benchmark.h"
#include "../engine/ecs/registry.h"
#include "../util/log.h"

namespace P3D::Engine {
class ECSGetFromRegistryBenchmark : public Benchmark {
public:
	ECSGetFromRegistryBenchmark() : Benchmark("ecsGetFromRegistryBenchmark") {}

	Registry64 registry;
	std::vector<int> v{1, 2, 3, 4, 5, 6};
	int errors = 0;
	
	struct A : public RC {
		int i; 
		
		A(int i) : i(i) {}
	};

	void init() override {
		int amount = 1000000;
		for(int i = 0; i < amount; i++) {
			auto id = registry.create();
			registry.add<A>(id, i);
		}
	}

	void run() override {
		int i = 0;
		auto view = registry.view<A>();
		for(auto entity : view) {
			auto& comp = *registry.get<A>(entity);
			if(comp.i != i)
				errors++;
			i++;
		}
	}

	void printResults(double timeTaken) override {
		Log::error("Amount of errors: %d\n", errors);
	}

} ecsGetFromRegistryBenchmark;

class ECSGetFromViewConjunctionBenchmark : public Benchmark {
public:
	ECSGetFromViewConjunctionBenchmark() : Benchmark("ecsGetFromViewConjunctionBenchmark") {}

	Registry64 registry;
	int errors = 0;

	struct A : public RC { int i; A(int i) : i(i) {} };

	void init() override {
		int amount = 1000000;
		for(int i = 0; i < amount; i++) {
			auto id = registry.create();
			registry.add<A>(id, i);
		}
	}

	void run() override {
		int i = 0;
		auto view = registry.view<Registry64::conjunction<A>>();
		for(auto entity : view) {
			auto comp = view.get<A>(entity);
			if(comp->i != i)
				errors++;
			i++;
		}
	}

	void printResults(double timeTaken) override {
		Log::error("Amount of errors: %d\n", errors);
	}

} ecsGetFromViewConjunctionBenchmark;

/*class ECSGetFromViewDisjunctionBenchmark : public Benchmark {
public:
	ECSGetFromViewDisjunctionBenchmark() : Benchmark("ecsGetFromViewDisjunctionBenchmark") {}

	Registry64 registry;
	int errors = 0;

	struct A : public RefCountable { int i; A(int i) : i(i) {} };

	void init() override {
		int amount = 1000000;
		for (int i = 0; i < amount; i++) {
			auto id = registry.create();
			registry.add<A>(id, i);
		}
	}

	void run() override {
		int i = 0;
		auto view = registry.view(Registry64::disjunction<A>());
		for (auto entity : view) {
			auto comp = view.get<A>(entity);
			if (comp->i != i)
				errors++;
			i++;
		}
	}

	void printResults(double timeTaken) override {
		Log::error("Amount of errors: %d\n", errors);
	}

} ecsGetFromViewDisjunctionBenchmark;*/
};