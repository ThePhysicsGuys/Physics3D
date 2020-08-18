#pragma once

#include <chrono>
#include <thread>

namespace P3D::Application {

using namespace std::chrono;

class TickerThread {
private:
	std::thread thread;
	bool stopped = false;
	double TPS;
	double speed = 1.0;
	milliseconds tickSkipTimeout;
	void(*tickAction)();
public:
	TickerThread() : thread(), TPS(0.0), tickSkipTimeout(0), tickAction(nullptr) {};
	TickerThread(double targetTPS, milliseconds tickSkipTimeout, void(*tickAction)());
	~TickerThread();

	TickerThread& operator=(TickerThread&& rhs) noexcept {
		this->thread = std::thread();
		this->stopped = rhs.stopped;
		this->TPS = rhs.TPS;
		this->tickSkipTimeout = rhs.tickSkipTimeout;
		this->tickAction = rhs.tickAction;

		return *this;
	}

	TickerThread& operator=(const TickerThread&) = delete;

	void start();
	void stop();

	void setTPS(double newTPS) { this->TPS = newTPS; }
	double getTPS() const { return this->TPS; }

	void setSpeed(double newSpeed) { this->speed = newSpeed; }
	double getSpeed() { return this->speed; }

	void runTick();
};

};