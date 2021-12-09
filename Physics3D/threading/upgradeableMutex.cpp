#include "upgradeableMutex.h"

// disable "Caller failing to hold lock 'this->writerLockout' before calling function 'std::_Mutex_base::unlock'" warning. 
#pragma warning(disable : 26110)

namespace P3D {
// Unlocked -> Exclusive
void UpgradeableMutex::lock() {
	writerLockout.lock();
	stateMutex.lock();
}

// Exclusive -> Unlocked
void UpgradeableMutex::unlock() {
	stateMutex.unlock();
	writerLockout.unlock();
}

// Unlocked -> Shared
void UpgradeableMutex::lock_shared() {
	stateMutex.lock_shared();
}

// Shared -> Unlocked
void UpgradeableMutex::unlock_shared() {
	stateMutex.unlock_shared();
}

// Unlocked -> Shared Upgradeable
void UpgradeableMutex::lock_upgradeable() {
	writerLockout.lock();
	stateMutex.lock_shared();
}

// Shared Upgradeable -> Unlocked
void UpgradeableMutex::unlock_upgradeable() {
	stateMutex.unlock_shared();
	writerLockout.unlock();
}

// Shared Upgradeable -> Exclusive
void UpgradeableMutex::upgrade() {
	stateMutex.unlock_shared();
	stateMutex.lock();
}

// Exclusive -> Shared Upgradeable
void UpgradeableMutex::downgrade() {
	stateMutex.unlock();
	stateMutex.lock_shared();
}

// Exclusive -> Shared
void UpgradeableMutex::final_downgrade() {
	stateMutex.unlock();
	stateMutex.lock_shared();
	writerLockout.unlock();
}

// Shared Upgradeable -> Shared
void UpgradeableMutex::cancel_upgrade() {
	writerLockout.unlock();
}
};
