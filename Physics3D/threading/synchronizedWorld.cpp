#include "synchonizedWorld.h"

#include "../worldPhysics.h"

namespace P3D {
void SynchronizedWorldPrototype::tick(ThreadPool& threadPool) {

	SharedLockGuard mutLock(lock);

	findColissionsParallel(*this, this->curColissions, threadPool);

	physicsMeasure.mark(PhysicsProcess::EXTERNALS);
	applyExternalForces(*this);

	handleColissions(this->curColissions);

	intersectionStatistics.nextTally();

	handleConstraints(*this);

	physicsMeasure.mark(PhysicsProcess::WAIT_FOR_LOCK);
	mutLock.upgrade();

	update(*this);

	physicsMeasure.mark(PhysicsProcess::QUEUE);
	processQueue();

	physicsMeasure.mark(PhysicsProcess::WAIT_FOR_LOCK);
	mutLock.downgrade();

	physicsMeasure.mark(PhysicsProcess::QUEUE);
	processReadQueue();
}
}
