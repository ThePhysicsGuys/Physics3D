#include "core.h"

#include "tickerThread.h"

namespace P3D::Application {

using namespace std::chrono;
TickerThread::TickerThread(double targetTPS, milliseconds tickSkipTimeout, void(*tickAction)()) {
	this->TPS = targetTPS;
	this->tickSkipTimeout = tickSkipTimeout;
	this->tickAction = tickAction;
}

TickerThread::~TickerThread() {
	this->stop();
}

void TickerThread::start() {
	this->stopped = false;

	this->thread = std::thread([this] () {
		time_point<system_clock> nextTarget = system_clock::now();

		while (!(this->stopped)) {

			microseconds tickTime = microseconds((long long) (1000000 / (this->TPS * this->speed)));

			this->tickAction();

			nextTarget += tickTime;
			time_point<system_clock>  curTime = system_clock::now();
			if (curTime < nextTarget) {
				std::this_thread::sleep_until(nextTarget);
			} else {
				// We're behind schedule
				if (nextTarget < curTime - this->tickSkipTimeout) {
					Log::warn("Can't keep up! Skipping %d ticks!", (int) ((curTime - nextTarget).count() / (1E3 * this->tickSkipTimeout.count())));

					nextTarget = curTime;
				}
			}
		}
		});
}

void TickerThread::runTick() {
	this->tickAction();
}

void TickerThread::stop() {
	this->stopped = true;
	if (this->thread.joinable()) this->thread.join();
}

};