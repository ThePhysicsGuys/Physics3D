#pragma once

#include <shared_mutex>

class SharedLockGuard {
	std::shared_mutex& mutex;
	bool isHard = false;

public:
	inline SharedLockGuard(std::shared_mutex& mutex) : mutex(mutex) {
		mutex.lock_shared();
	}

	inline ~SharedLockGuard() {
		if(isHard) {
			mutex.unlock();
		} else {
			mutex.unlock_shared();
		}
	}

	inline void upgrade() {
		if(isHard) {
			throw "Attemt to upgrade already hard lock!";
		}
		mutex.unlock_shared();
		mutex.lock();
		isHard = true;
	}

	inline void downgrade() {
		if(!isHard) {
			throw "Attempt to downgrade already soft lock!";
		}
		mutex.unlock();
		mutex.lock_shared();
		isHard = false;
	}
};

class UnlockOnDestroy {
	std::shared_mutex& mutex;
public:
	/* assumes it is given a locked mutex
	Unlocks when destroyed*/
	inline UnlockOnDestroy(std::shared_mutex& mutex) : mutex(mutex) {}
	~UnlockOnDestroy() { mutex.unlock(); }
};

class UnlockSharedOnDestroy {
	std::shared_mutex& mutex;
public:
	/* assumes it is given a shared_locked mutex
	Unlocks when destroyed*/
	inline UnlockSharedOnDestroy(std::shared_mutex& mutex) : mutex(mutex) {}
	~UnlockSharedOnDestroy() { mutex.unlock_shared(); }
};