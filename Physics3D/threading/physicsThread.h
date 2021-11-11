#pragma once

#include <thread>
#include <atomic>

#include "threadPool.h"

namespace P3D {
class WorldPrototype;

class PhysicsThread {
	std::thread thread;
	ThreadPool threadPool;
	std::atomic<bool> shouldStop = true;

public:
	std::atomic<double> speed = 1.0;
	std::atomic<std::chrono::milliseconds> tickSkipTimeout;
	WorldPrototype* world;
	void(*tickFunction)(WorldPrototype*);

	PhysicsThread(std::chrono::milliseconds tickSkipTimeout = std::chrono::milliseconds(1000), unsigned int threadCount = 0);
	PhysicsThread(WorldPrototype* world, std::chrono::milliseconds tickSkipTimeout = std::chrono::milliseconds(1000), unsigned int threadCount = 0);
	PhysicsThread(void(&tickFunction)(WorldPrototype*), std::chrono::milliseconds tickSkipTimeout = std::chrono::milliseconds(1000), unsigned int threadCount = 0);
	PhysicsThread(WorldPrototype* world, void(&tickFunction)(WorldPrototype*), std::chrono::milliseconds tickSkipTimeout = std::chrono::milliseconds(1000), unsigned int threadCount = 0);
	~PhysicsThread();
	void runTick();
	void start();
	void stop();
};
}
