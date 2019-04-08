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

template<size_t N, typename Unit, typename Category>
class HistoricTally {
	ParallelArray<Unit, static_cast<size_t>(Category::COUNT)> currentTally;
public:
	char const * labels[static_cast<size_t>(Category::COUNT)];
	CircularBuffer<ParallelArray<Unit, static_cast<size_t>(Category::COUNT)>, N> history;

	inline void addToTally(Category category, Unit amount) {
		currentTally[static_cast<size_t>(category)] += amount;
	}

	inline void clearCurrentTally() {
		for(size_t i = 0; i < static_cast<size_t>(Category::COUNT); i++) {
			currentTally[i] = Unit(0);
		}
	}

	inline void nextTally() {
		history.add(currentTally);
		clearCurrentTally();
	}

	inline HistoricTally(char const * const labels[static_cast<size_t>(Category::COUNT)]) {
		for(size_t i = 0; i < static_cast<size_t>(Category::COUNT); i++) {
			this->labels[i] = labels[i];
		}
		clearCurrentTally();
	}

	inline size_t size() const {
		return static_cast<size_t>(Category::COUNT);
	}
};

template<size_t N, typename ProcessType>
class BreakdownAverageProfiler : public HistoricTally<N, std::chrono::nanoseconds, ProcessType> {
	std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::high_resolution_clock::now();
	ProcessType currentProcess = static_cast<ProcessType>(-1);

public:
	inline BreakdownAverageProfiler(char const * const labels[static_cast<size_t>(ProcessType::COUNT)]) : HistoricTally(labels) {}

	inline void mark(ProcessType process) {
		std::chrono::time_point<std::chrono::steady_clock> curTime = std::chrono::high_resolution_clock::now();
		if(currentProcess != static_cast<ProcessType>(-1)) {
			addToTally(currentProcess, curTime - startTime);
		}
		startTime = curTime;
		currentProcess = process;
	}

	inline void end() {
		std::chrono::time_point<std::chrono::steady_clock> curTime = std::chrono::high_resolution_clock::now();
		addToTally(currentProcess, curTime - startTime);

		currentProcess = static_cast<ProcessType>(-1);
		nextTally();
	}
};
