#pragma once

#include <chrono>
#include "buffers.h"

class TimerMeasure {
	std::chrono::time_point<std::chrono::steady_clock> lastClock = std::chrono::high_resolution_clock::now();
public:
	CircularBuffer<std::chrono::nanoseconds, 300> pastTickLengths;
	CircularBuffer<std::chrono::nanoseconds, 60> pastBetweenTimes;
	inline void start() {
		std::chrono::time_point<std::chrono::steady_clock> curTime = std::chrono::high_resolution_clock::now();
		std::chrono::nanoseconds timeSinceLast = curTime - lastClock;
		pastBetweenTimes.add(timeSinceLast);
		lastClock = curTime;
	}
	inline void end() {
		std::chrono::nanoseconds timeTaken = std::chrono::high_resolution_clock::now() - lastClock;
		pastTickLengths.add(timeTaken);
	}
};

extern TimerMeasure physicsMeasure;
extern TimerMeasure frameMeasure;
