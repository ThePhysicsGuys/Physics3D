#pragma once

#include <thread>
#include <atomic>

#include "threadPool.h"
#include "upgradeableMutex.h"

namespace P3D {
class WorldPrototype;

class PhysicsThread {
	std::thread thread;
	ThreadPool threadPool;
	std::atomic<bool> shouldBeRunning = false;

public:
	std::atomic<double> speed = 1.0;
	std::atomic<std::chrono::milliseconds> tickSkipTimeout;
	WorldPrototype* world;
	UpgradeableMutex* worldMutex;
	void(*tickFunction)(WorldPrototype*);

	PhysicsThread(WorldPrototype* world, UpgradeableMutex* worldMutex, void(&tickFunction)(WorldPrototype*), std::chrono::milliseconds tickSkipTimeout = std::chrono::milliseconds(1000), unsigned int threadCount = 0);
	PhysicsThread(WorldPrototype* world, UpgradeableMutex* worldMutex, std::chrono::milliseconds tickSkipTimeout = std::chrono::milliseconds(1000), unsigned int threadCount = 0);
	PhysicsThread(void(&tickFunction)(WorldPrototype*), std::chrono::milliseconds tickSkipTimeout = std::chrono::milliseconds(1000), unsigned int threadCount = 0);
	PhysicsThread(std::chrono::milliseconds tickSkipTimeout = std::chrono::milliseconds(1000), unsigned int threadCount = 0);
	~PhysicsThread();
	// Runs one tick. The PhysicsThread must not be running!
	void runTick();
	// Starts the PhysicsThread
	void start();
	// Stops the PhysicsThread, and returns once it has been stopped completely
	void stop();
	// instructs the PhysicsThread to stop, but returns immediately. PhysicsThread may still be running for some time before finally stopping
	void stopAsync();
	bool isRunning();
	// Starts if isRunning() == false, stop() if isRunning() == true
	void toggleRunning();
	// Starts if isRunning() == false, stopAsync() if isRunning() == true
	void toggleRunningAsync();
};
}
