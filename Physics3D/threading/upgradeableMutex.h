#pragma once

#include <mutex>
#include <shared_mutex>

namespace P3D {
/*
This mutex allows minimal reader interference for operations that require read/write exclusivity.
It allows a read lock with promise of upgrading to a write lock later, ensuring no writers in between.
For everything else, acts like a shared_mutex

Each actor has 4 states:
	- Unlocked
	- Shared
	- Shared Upgradeable
	- Exclusive

Most basic usage:
mtx.lock_shared_upgradeable();
// Shared read operations here, writers must wait until unlock.
// Build some state here from reading, other readers allowed.
mtx.upgrade();
// Write here, other readers are excluded, other writers are exluded.
// State is consistent with read section.
mtx.unlock();

Read-Write-Read usage:
mtx.lock_shared_upgradeable();
// Shared read operations here, writers must wait until unlock.
// Build some state here from reading, other readers allowed.
mtx.upgrade();
// Write here, other readers are excluded, other writers are exluded.
// State is consistent with read section.
mtx.downgrade();
// Read section, still maintain upgradability, other readers allowed.
mtx.upgrade();
// Write section
mtx.final_downgrade();
// Read section again, other readers allowed. Writers still excluded.
// State is consistent with previous read and write sections.
mtx.unlock_shared();
// Other writers now allowed.


All function names are formatted snake_case to be compatible with std::lock and std::shared_lock
*/
class UpgradeableMutex {
	std::shared_mutex stateMutex;

	// Only one thread can be upgrading or have an exclusive lock at a time
	std::mutex writerLockout;

public:
	// Unlocked -> Exclusive
	void lock();
	// Exclusive -> Unlocked
	void unlock();
	
	// Unlocked -> Shared
	void lock_shared();
	// Shared -> Unlocked
	void unlock_shared();
	
	// Unlocked -> Shared Upgradeable
	void lock_upgradeable();
	// Shared Upgradeable -> Unlocked
	void unlock_upgradeable();

	// Shared Upgradeable -> Exclusive
	void upgrade();
	// Exclusive -> Shared Upgradeable
	void downgrade();
	// Exclusive -> Shared
	void final_downgrade(); // calling final_downgrade instead of downgrade will allow another lock_upgradeable to make a reservation
	// Shared Upgradeable -> Shared
	void cancel_upgrade();
};
};
