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

class WorldPrototype : public PartContainer {
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

	WorldPrototype();

	WorldPrototype(const WorldPrototype&) = delete;
	WorldPrototype(WorldPrototype&&) = delete;
	WorldPrototype& operator=(const WorldPrototype&) = delete;
	WorldPrototype& operator=(WorldPrototype&&) = delete;

	void tick(double deltaT);

	void addObject(Part* p, bool anchored = false);
	void removePart(Part* p);
	virtual void applyExternalForces();
	bool isValid() const;
};

template<typename T>
class CustomPartIter {
	PartIterator iterator;
public:
	inline CustomPartIter(Part** current) : iterator(current) {}
	inline T& operator*() const {
		return static_cast<T&>(*iterator);
	}

	inline CustomPartIter& operator++() {
		++iterator;
		return *this;
	}

	inline bool operator!=(CustomPartIter& other) { return this->iterator != other.iterator; }
	inline bool operator==(CustomPartIter& other) { return this->iterator == other.iterator; }
};

template<typename T = Part>
class World : public WorldPrototype {
public:
	inline CustomPartIter<T> begin() { return CustomPartIter<T>(parts); }
	inline CustomPartIter<T> end() { return CustomPartIter<T>(parts + partCount); }
};
