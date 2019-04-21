#pragma once

#include "part.h"
#include "physical.h"
#include <vector>
#include <queue>
#include <mutex>
#include <shared_mutex>

#include "partContainer.h"

struct QueuedPart {
	Part* p;
	bool anchored;
};

class World : public PartContainer {
private:
	std::queue<QueuedPart> newPartQueue;
	size_t getTotalVertexCount();
	void processQueue();
	void processColissions();

	void addPartUnsafe(Part* p, bool anchored);
public:
	mutable std::shared_mutex lock;
	mutable std::mutex queueLock;

	Physical* selectedPhysical = nullptr;
	Vec3 localSelectedPoint;
	Vec3 magnetPoint;

	World();

	World(const World&) = delete;
	World(World&&) = delete;
	World& operator=(const World&) = delete;
	World& operator=(World&&) = delete;

	void tick(double deltaT);

	void addObject(Part* p, bool anchored = false);
	inline void addObject(Part p, bool anchored = false) { addObject(new Part(p), anchored); }
	void removePart(Part* p);
	virtual void applyExternalForces();
	bool isValid() const;
};

template<typename T = Part>
class ManagedWorld : public World {
public:
	
};
