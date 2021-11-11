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
	WorldPrototype* world = nullptr;
	void(*tickFunction)(WorldPrototype*);

public:
	std::atomic<double> speed = 1.0;
	std::atomic<std::chrono::milliseconds> tickSkipTimeout;

	PhysicsThread(void(&tickFunction)(WorldPrototype*), std::chrono::milliseconds tickSkipTimeout = std::chrono::milliseconds(1000), unsigned int threadCount = 0);
	~PhysicsThread();
	void setWorld(WorldPrototype* newWorld);
	void runTick();
	void start();
	void stop();
};
}
