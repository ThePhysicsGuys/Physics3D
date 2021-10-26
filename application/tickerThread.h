#pragma once

#include <chrono>
#include <thread>
#include <atomic>

namespace P3D::Application {

class TickerThread {
private:
	std::thread thread;
	std::atomic<bool> stopped = false;
	std::atomic<double> TPS;
	std::atomic<double> speed = 1.0;
	std::chrono::milliseconds tickSkipTimeout;
	void(*tickAction)();
public:
	TickerThread() : thread(), TPS(0.0), tickSkipTimeout(0), tickAction(nullptr) {};
	TickerThread(double targetTPS, std::chrono::milliseconds tickSkipTimeout, void(*tickAction)());
	~TickerThread();

	TickerThread(TickerThread&& other) = default;
	TickerThread& operator=(TickerThread&& other) = default;

	void start();
	void stop();

	void setTPS(double newTPS) { this->TPS = newTPS; }
	double getTPS() const { return this->TPS; }

	void setSpeed(double newSpeed) { this->speed = newSpeed; }
	double getSpeed() { return this->speed; }

	void runTick();
};

};