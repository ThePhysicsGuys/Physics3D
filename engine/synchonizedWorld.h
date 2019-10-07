#pragma once

#include <queue>
#include <mutex>
#include <shared_mutex>
#include <functional>

#include "world.h"
#include "sharedLockGuard.h"
#include "physicsProfiler.h"

template<typename T = Part>
class SynchronizedWorld : public World<T> {
	mutable std::shared_mutex lock;
	mutable std::mutex queueLock;
	mutable std::mutex readQueueLock;

	std::queue<std::function<void()>> waitingOperations;
	mutable std::queue<std::function<void()>> waitingReadOnlyOperations;

	void pushOperation(const std::function<void()>& func) {
		std::lock_guard<std::mutex> lg(queueLock);
		waitingOperations.push(func);
	}
	void pushReadOnlyOperation(const std::function<void()>& func) const {
		std::lock_guard<std::mutex> lg(readQueueLock);
		waitingReadOnlyOperations.push(func);
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

	void syncModification(const std::function<void()>& function) {
		std::lock_guard<std::shared_mutex> lg(lock);
		function();
	}
	void asyncModification(const std::function<void()>& function) {
		if (lock.try_lock()) {
			UnlockOnDestroy lg(lock);
			function();
		} else {
			pushOperation(function);
		}
	}
	void syncReadOnlyOperation(const std::function<void()>& function) const {
		SharedLockGuard lg(lock);
		function();
	}
	void asyncReadOnlyOperation(const std::function<void()>& function) const {
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
