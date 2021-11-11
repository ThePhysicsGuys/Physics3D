#include "physicsThread.h"
#include "../world.h"
#include "../worldPhysics.h"
#include "../misc/physicsProfiler.h"

#include <iostream>

namespace P3D {
using namespace std::chrono;

PhysicsThread::PhysicsThread(void(&tickFunction)(WorldPrototype*), std::chrono::milliseconds tickSkipTimeout, unsigned int threadCount) : 
	tickFunction(tickFunction),
	tickSkipTimeout(tickSkipTimeout),
	threadPool(threadCount == 0 ? std::thread::hardware_concurrency() : threadCount) {}

PhysicsThread::~PhysicsThread() {
	this->stop();
}

void PhysicsThread::setWorld(WorldPrototype* newWorld) {
	this->world = newWorld;
}

void PhysicsThread::runTick() {
	physicsMeasure.mark(PhysicsProcess::OTHER);

	this->world->tick(this->threadPool);
	tickFunction(this->world);

	physicsMeasure.end();

	GJKCollidesIterationStatistics.nextTally();
	GJKNoCollidesIterationStatistics.nextTally();
	EPAIterationStatistics.nextTally();
}

void PhysicsThread::start() {
	this->shouldStop = false;

	this->thread = std::thread([this] () {
		time_point<system_clock> nextTarget = system_clock::now();

		while (!(this->shouldStop)) {

			microseconds tickTime = microseconds((long long) (1000000 * this->world->deltaT / this->speed));

			this->runTick();

			nextTarget += tickTime;
			time_point<system_clock>  curTime = system_clock::now();
			if (curTime < nextTarget) {
				std::this_thread::sleep_until(nextTarget);
			} else {
				// We're behind schedule
				milliseconds tickSkipTimeout = this->tickSkipTimeout.load();
				if (nextTarget < curTime - tickSkipTimeout) {
					int ticksToSkip = (curTime - nextTarget).count() / (1000.0 * tickSkipTimeout.count());
					std::cout << "Can't keep up! Skipping " << ticksToSkip << " ticks!";

					nextTarget = curTime;
				}
			}
		}
	});
}
void PhysicsThread::stop() {
	this->shouldStop = true;
	this->thread.join();
}
}

