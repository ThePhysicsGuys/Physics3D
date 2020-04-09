#pragma once

#include <chrono>
#include <map>

#include "datastructures/buffers.h"
#include "parallelArray.h"

class TimerMeasure {
	std::chrono::high_resolution_clock::time_point lastClock = std::chrono::high_resolution_clock::now();
public:
	CircularBuffer<std::chrono::nanoseconds> pastTickLengths;
	CircularBuffer<std::chrono::nanoseconds> pastBetweenTimes;

	TimerMeasure(size_t capacity) : pastTickLengths(capacity), pastBetweenTimes(capacity) {}

	inline void start() {
		std::chrono::high_resolution_clock::time_point curTime = std::chrono::high_resolution_clock::now();
		std::chrono::nanoseconds timeSinceLast = curTime - lastClock;
		pastBetweenTimes.add(timeSinceLast);
		lastClock = curTime;
	}
	inline void end() {
		std::chrono::nanoseconds timeTaken = std::chrono::high_resolution_clock::now() - lastClock;
		pastTickLengths.add(timeTaken);
	}
};

template<typename Unit, typename Category>
class HistoricTally {
	ParallelArray<Unit, static_cast<size_t>(Category::COUNT)> currentTally;
public:
	char const * labels[static_cast<size_t>(Category::COUNT)];
	CircularBuffer<ParallelArray<Unit, static_cast<size_t>(Category::COUNT)>> history;

	inline HistoricTally(char const* const labels[static_cast<size_t>(Category::COUNT)], size_t size) : history(size) {
		for(size_t i = 0; i < static_cast<size_t>(Category::COUNT); i++) {
			this->labels[i] = labels[i];
		}
		clearCurrentTally();
	}

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

	constexpr inline size_t size() const {
		return static_cast<size_t>(Category::COUNT);
	}
};

template<typename ProcessType>
class BreakdownAverageProfiler : public HistoricTally<std::chrono::nanoseconds, ProcessType> {
	std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
	ProcessType currentProcess = static_cast<ProcessType>(-1);


public:
	CircularBuffer<std::chrono::high_resolution_clock::time_point> tickHistory;

	inline BreakdownAverageProfiler(char const * const labels[static_cast<size_t>(ProcessType::COUNT)], size_t capacity) : HistoricTally<std::chrono::nanoseconds, ProcessType>(labels, capacity), tickHistory(capacity) {}

	inline void mark(ProcessType process) {
		std::chrono::high_resolution_clock::time_point curTime = std::chrono::high_resolution_clock::now();
		if(currentProcess != static_cast<ProcessType>(-1)) {
			HistoricTally<std::chrono::nanoseconds, ProcessType>::addToTally(currentProcess, curTime - startTime);
		}
		startTime = curTime;
		currentProcess = process;
	}

	inline void mark(ProcessType process, ProcessType overrideOldProcess) {
		std::chrono::high_resolution_clock::time_point curTime = std::chrono::high_resolution_clock::now();
		if (currentProcess != static_cast<ProcessType>(-1)) {
			HistoricTally<std::chrono::nanoseconds, ProcessType>::addToTally(overrideOldProcess, curTime - startTime);
		}
		startTime = curTime;
		currentProcess = process;
	}

	inline void end() {
		std::chrono::high_resolution_clock::time_point curTime = std::chrono::high_resolution_clock::now();
		this->addToTally(currentProcess, curTime - startTime);
		tickHistory.add(curTime);

		currentProcess = static_cast<ProcessType>(-1);
		this->nextTally();
	}

	inline double getAvgTPS() {
		size_t numTicks = tickHistory.size();
		if(numTicks != 0) {
			std::chrono::high_resolution_clock::time_point firstTime = tickHistory.tail();
			std::chrono::high_resolution_clock::time_point lastTime = tickHistory.front();
			std::chrono::nanoseconds delta = lastTime - firstTime;

			double timeTaken = delta.count() * 1E-9;

			return (numTicks-1) / timeTaken;
		}
		return 0.0;
	}
};
