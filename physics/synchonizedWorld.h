#pragma once

#include <queue>
#include <mutex>
#include <shared_mutex>
#include <functional>

#include "world.h"
#include "sharedLockGuard.h"
#include "misc/physicsProfiler.h"

template<typename T = Part>
class SynchronizedWorld : public World<T> {
	mutable std::shared_mutex lock;
	mutable std::mutex queueLock;
	mutable std::mutex readQueueLock;

	std::queue<std::function<void()>> waitingOperations;
	mutable std::queue<std::function<void()>> waitingReadOnlyOperations;

	template<typename Func>
	void pushOperation(const Func& func) {
		std::lock_guard<std::mutex> lg(queueLock);
		waitingOperations.emplace(func);
	}
	template<typename Func>
	void pushReadOnlyOperation(const Func& func) const {
		std::lock_guard<std::mutex> lg(readQueueLock);
		waitingReadOnlyOperations.emplace(func);
	}

	void processQueue() {
		std::lock_guard<std::mutex> lg(queueLock);

		while (!waitingOperations.empty()) {
			std::function<void()>& operation = waitingOperations.front();
			operation();
			waitingOperations.pop();
		}
	}

	void processReadQueue() const {
		std::lock_guard<std::mutex> lg(readQueueLock);

		while (!waitingReadOnlyOperations.empty()) {
			std::function<void()>& operation = waitingReadOnlyOperations.front();
			operation();
			waitingReadOnlyOperations.pop();
		}
	}

public:

	SynchronizedWorld<T>(double deltaT) : World<T>(deltaT) {}

	template<typename Func>
	void syncModification(const Func& function) {
		std::lock_guard<std::shared_mutex> lg(lock);
		function();
	}
	template<typename Func>
	void asyncModification(const Func& function) {
		if (lock.try_lock()) {
			UnlockOnDestroy lg(lock);
			function();
		} else {
			pushOperation(function);
		}
	}
	template<typename Func>
	void syncReadOnlyOperation(const Func& function) const {
		SharedLockGuard lg(lock);
		function();
	}
	template<typename Func>
	void asyncReadOnlyOperation(const Func& function) const {
		if (lock.try_lock_shared()) {
			UnlockSharedOnDestroy lg(lock);
			function();
		} else {
			pushReadOnlyOperation(function);
		}
	}

	virtual void tick() override {
		SharedLockGuard mutLock(lock);
		
		this->findColissions();
		
		physicsMeasure.mark(PhysicsProcess::EXTERNALS);
		this->applyExternalForces();

		this->handleColissions();

		intersectionStatistics.nextTally();

		this->handleConstraints();

		physicsMeasure.mark(PhysicsProcess::WAIT_FOR_LOCK);
		mutLock.upgrade();
		this->update();

		physicsMeasure.mark(PhysicsProcess::QUEUE);
		processQueue();
		
		physicsMeasure.mark(PhysicsProcess::WAIT_FOR_LOCK);
		mutLock.downgrade();

		physicsMeasure.mark(PhysicsProcess::QUEUE);
		processReadQueue();
	}
};
