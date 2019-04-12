#pragma once

#include "part.h"
#include "physical.h"
#include <vector>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <iterator>

class PhysicalContainer {
private:
	size_t capacity;
	void movePhysical(size_t origin, size_t destination);
	void swapPhysical(size_t first, size_t second);
public:
	size_t physicalCount;
	size_t freePhysicalsOffset;
	Physical* physicals;

	PhysicalContainer(size_t initialCapacity);
	void ensureCapacity(size_t capacity);
	void add(Physical& p);
	void addAnchored(Physical& p);
	void remove(size_t index);
	void anchor(size_t index);
	void unanchor(size_t index);
	bool isAnchored(size_t index);

	inline Physical& operator[](size_t index) {
		return physicals[index];
	}
	inline const Physical& operator[](size_t index) const {
		return physicals[index];
	}
	inline Physical* begin() {
		return physicals;
	}
	inline const Physical* begin() const {
		return physicals;
	}
	inline Physical* end() {
		return physicals + physicalCount;
	}
	inline const Physical* end() const {
		return physicals + physicalCount;
	}

	template<typename T>
	struct Iter {
		T* first; T* last;
		T* begin() { return first; }
		T* end() { return last; }
	};
	template<typename T>
	struct ConstIter {
		const T* first; const T* last;
		const T* begin() const { return first; }
		const T* end() const { return last; }
	};

	Iter<Physical> iterPhysicals() { return Iter<Physical>{physicals, physicals+physicalCount}; }
	ConstIter<Physical> iterPhysicals() const { return ConstIter<Physical>{physicals, physicals + physicalCount}; }

	Iter<Physical> iterAnchoredPhysicals() { return Iter<Physical>{physicals, physicals + freePhysicalsOffset}; }
	ConstIter<Physical> iterAnchoredPhysicals() const { return ConstIter<Physical>{physicals, physicals + freePhysicalsOffset}; }

	Iter<Physical> iterUnAnchoredPhysicals() { return Iter<Physical>{physicals + freePhysicalsOffset, physicals + physicalCount}; }
	ConstIter<Physical> iterUnAnchoredPhysicals() const { return ConstIter<Physical>{physicals + freePhysicalsOffset, physicals + physicalCount}; }
};

class World {
private:
	size_t getTotalVertexCount();
	void processQueue();
	void processColissions(const Shape* transformedShapes);
public:
	PhysicalContainer physicals;
	std::queue<Physical> newPhysicalQueue;
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
	void addObject(Physical& p);
	Physical& addObject(Shape s, CFrame location, double density, double friction);
	Physical& addObject(Part& p);

	virtual void applyExternalForces(const Shape* transformedShapes);
};
