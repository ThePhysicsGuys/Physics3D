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

	double getTotalKineticEnergy() const;
	virtual double getTotalPotentialEnergy() const;
	virtual double getPotentialEnergyOfPhysical(const Physical& p) const;
	double getTotalEnergy() const;
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
template<typename T>
class ConstCustomPartIter {
	ConstPartIterator iterator;
public:
	inline ConstCustomPartIter(const Part * const * current) : iterator(current) {}
	inline const T& operator*() const {
		return static_cast<const T&>(*iterator);
	}

	inline ConstCustomPartIter& operator++() {
		++iterator;
		return *this;
	}

	inline bool operator!=(ConstCustomPartIter& other) { return this->iterator != other.iterator; }
	inline bool operator==(ConstCustomPartIter& other) { return this->iterator == other.iterator; }
};

template<typename T>
class CustomPartIteratorFactory {
	Part** start;
	Part** finish;
public:
	inline CustomPartIteratorFactory(Part** start, Part** finish) : start(start), finish(finish) {}
	inline CustomPartIter<T> begin() const { return CustomPartIter<T>(start); }
	inline CustomPartIter<T> end() const { return CustomPartIter<T>(finish); }
};

template<typename T>
class ConstCustomPartIteratorFactory {
	const Part* const * start;
	const Part* const * finish;
public:
	inline ConstCustomPartIteratorFactory(const Part* const * start, const Part* const * finish) : start(start), finish(finish) {}
	inline ConstCustomPartIter<T> begin() const { return ConstCustomPartIter<T>(start); }
	inline ConstCustomPartIter<T> end() const { return ConstCustomPartIter<T>(finish); }
};

template<typename T = Part>
class World : public WorldPrototype {
public:
	inline CustomPartIter<T> begin() { return CustomPartIter<T>(parts); }
	inline CustomPartIter<T> end() { return CustomPartIter<T>(parts + partCount); }

	inline CustomPartIteratorFactory<T> iterAnchoredParts() { return CustomPartIteratorFactory<T>(parts, parts + anchoredPartsCount); }
	inline ConstCustomPartIteratorFactory<T> iterAnchoredParts() const { return ConstCustomPartIteratorFactory<T>(parts, parts + anchoredPartsCount); }

	inline CustomPartIteratorFactory<T> iterFreeParts() { return CustomPartIteratorFactory<T>(parts + anchoredPartsCount, parts + partCount); }
	inline ConstCustomPartIteratorFactory<T> iterFreeParts() const { return ConstCustomPartIteratorFactory<T>(parts + anchoredPartsCount, parts + partCount); }
};

double computeCombinedInertiaBetween(const Physical& first, const Physical& second, const Vec3& localColissionFirst, const Vec3& localColissionSecond, const Vec3& colissionNormal);
