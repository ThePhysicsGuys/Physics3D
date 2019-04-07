#pragma once

#include <chrono>
#include <map>

#include "circularBuffer.h"
#include "parallelArray.h"

template<size_t N>
class TimerMeasure {
	std::chrono::time_point<std::chrono::steady_clock> lastClock = std::chrono::high_resolution_clock::now();
public:
	CircularBuffer<std::chrono::nanoseconds, N> pastTickLengths;
	CircularBuffer<std::chrono::nanoseconds, N> pastBetweenTimes;
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

template<size_t N, typename ProcessType>
class BreakdownAverageProfiler {
	std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::high_resolution_clock::now();
	ProcessType currentProcess = static_cast<ProcessType>(-1);
	ParallelArray<std::chrono::nanoseconds, static_cast<size_t>(ProcessType::COUNT)> currentTally;

	inline void clearCurrentTally() {
		for(int i = 0; i < static_cast<size_t>(ProcessType::COUNT); i++) {
			currentTally[i] = std::chrono::nanoseconds(0);
		}
	}

public:
	char const * labels[static_cast<size_t>(ProcessType::COUNT)];
	CircularBuffer<ParallelArray<std::chrono::nanoseconds, static_cast<size_t>(ProcessType::COUNT)>, N> history;

	inline BreakdownAverageProfiler(char const * const labels[static_cast<size_t>(ProcessType::COUNT)]) {
		for(int i = 0; i < static_cast<size_t>(ProcessType::COUNT); i++) {
			this->labels[i] = labels[i];
		}
		clearCurrentTally();
	}

	inline void mark(ProcessType process) {
		std::chrono::time_point<std::chrono::steady_clock> curTime = std::chrono::high_resolution_clock::now();
		if(currentProcess != static_cast<ProcessType>(-1)) {
			std::chrono::nanoseconds delta = curTime - startTime;
			currentTally[static_cast<size_t>(currentProcess)] += delta;
		}
		startTime = curTime;
		currentProcess = process;
	}

	inline void end() {
		std::chrono::time_point<std::chrono::steady_clock> curTime = std::chrono::high_resolution_clock::now();
		std::chrono::nanoseconds delta = curTime - startTime;
		currentTally[static_cast<size_t>(currentProcess)] += delta;
		currentProcess = static_cast<ProcessType>(-1);

		history.add(currentTally);

		clearCurrentTally();
	}

	inline size_t size() const {
		return static_cast<size_t>(ProcessType::COUNT);
	}
};
