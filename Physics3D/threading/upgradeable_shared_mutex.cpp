#include "upgradeable_shared_mutex.h"

#pragma warning(disable : 26110)

namespace P3D {
// Unlocked -> Exclusive
void upgradeable_shared_mutex::lock() {
	writerLockout.lock(); // lock dibs to make sure no upgrader is currently waiting for this
	stateMutex.lock();
}

// Exclusive -> Unlocked
void upgradeable_shared_mutex::unlock() {
	stateMutex.unlock();
	writerLockout.unlock();
}

// Unlocked -> Shared
void upgradeable_shared_mutex::lock_shared() {
	stateMutex.lock_shared();
}

// Shared -> Unlocked
void upgradeable_shared_mutex::unlock_shared() {
	stateMutex.unlock_shared();
}

// Unlocked -> Shared Upgradeable
void upgradeable_shared_mutex::lock_shared_upgradeable() {
	writerLockout.lock();
	stateMutex.lock_shared();
}

// Shared Upgradeable -> Unlocked
void upgradeable_shared_mutex::unlock_shared_upgradeable() {
	stateMutex.unlock_shared();
	writerLockout.unlock();
}

// Shared Upgradeable -> Exclusive
void upgradeable_shared_mutex::upgrade() {
	stateMutex.unlock_shared();
	stateMutex.lock();
}

// Exclusive -> Shared Upgradeable
void upgradeable_shared_mutex::downgrade() {
	stateMutex.unlock();
	stateMutex.lock_shared();
}

// Exclusive -> Shared
void upgradeable_shared_mutex::final_downgrade() {
	stateMutex.unlock();
	stateMutex.lock_shared();
	writerLockout.unlock();
}

// Shared Upgradeable -> Shared
void upgradeable_shared_mutex::cancel_upgrade() {
	writerLockout.unlock();
}
};
