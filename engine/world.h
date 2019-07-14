#pragma once

#include "part.h"
#include "physical.h"
#include <vector>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include "datastructures/splitUnorderedList.h"
#include "worldIterator.h"
#include "datastructures/boundsTree.h"

class WorldPrototype {
private:
	std::queue<std::function<void(WorldPrototype*)>> waitingOperations;
	SplitUnorderedList<Physical*> physicals;

	size_t getTotalVertexCount();
	void processQueue();

	void addPartUnsafe(Part* p, bool anchored);
	void removePartUnsafe(Part* p);
	void attachPartUnsafe(Part* part, Physical& phys, CFrame attachment);
	void detachPartUnsafe(Part* part);
	void pushOperation(const std::function<void(WorldPrototype*)>& operation);
public:
	mutable std::shared_mutex lock;
	mutable std::mutex queueLock;
	BoundsTree objectTree;

	Part* selectedPart = nullptr;

	WorldPrototype();
	WorldPrototype(size_t initialPartCapacity);

	WorldPrototype(const WorldPrototype&) = delete;
	WorldPrototype(WorldPrototype&&) = delete;
	WorldPrototype& operator=(const WorldPrototype&) = delete;
	WorldPrototype& operator=(WorldPrototype&&) = delete;

	void tick(double deltaT);

	void addPart(Part* p, bool anchored = false);
	void attachPart(Part* p, Physical& phys, CFrame attachment);
	void detachPart(Part* p);
	void removePart(Part* p);

	inline bool isAnchored(Physical* p) const {
		// return physicals.isLeftSide(p);
		return false;
	}

	inline void anchor(Physical* p) {
		// if (!isAnchored(p)) physicals.moveRightToLeft(p);
	}

	inline void unanchor(Physical* p) {
		// if (isAnchored(p)) physicals.moveLeftToRight(p);
	}

	inline size_t getPartCount() const {
		size_t total = 0;
		for (const Physical& p : iterPhysicals()) {
			total += p.getPartCount();
		}
		return total;
	}

	inline size_t getAnchoredPartCount() const {
		size_t total = 0;
		for (const Physical& p : iterAnchoredPhysicals()) {
			total += p.getPartCount();
		}
		return total;
	}

	inline size_t getFreePartCount() const {
		size_t total = 0;
		for (const Physical& p : iterFreePhysicals()) {
			total += p.getPartCount();
		}
		return total;
	}

	virtual void applyExternalForces();
	bool isValid() const;

	double getTotalKineticEnergy() const;
	virtual double getTotalPotentialEnergy() const;
	virtual double getPotentialEnergyOfPhysical(const Physical& p) const;
	double getTotalEnergy() const;



	ListOfPtrIterFactory<Physical> iterPhysicals() { return ListOfPtrIterFactory<Physical>{ physicals.begin(), physicals.end() }; }
	ListOfPtrIterFactory<const Physical> iterPhysicals() const { return ListOfPtrIterFactory<const Physical>{ physicals.begin(), physicals.end() }; }

	ListOfPtrIterFactory<Physical> iterAnchoredPhysicals() { return ListOfPtrIterFactory<Physical>(physicals.begin(), physicals.getSplitOffset()); }
	ListOfPtrIterFactory<const Physical> iterAnchoredPhysicals() const { return ListOfPtrIterFactory<const Physical>(physicals.begin(), physicals.getSplitOffset()); }

	ListOfPtrIterFactory<Physical> iterFreePhysicals() { return ListOfPtrIterFactory<Physical>(physicals.getSplitOffset(), physicals.end()); }
	ListOfPtrIterFactory<const Physical> iterFreePhysicals() const { return ListOfPtrIterFactory<const Physical>(physicals.getSplitOffset(), physicals.end()); }
};

template<typename T = Part>
class World : public WorldPrototype {
public:
	inline CustomPartIter<T> begin() { return CustomPartIter<T>(PartIterator(iterPhysicals().begin())); }
	inline CustomPartIter<T> end() { return CustomPartIter<T>(PartIterator(iterPhysicals().end(), PartIter())); }

	inline CustomPartIteratorFactory<T> iterAnchoredParts() { return CustomPartIteratorFactory<T>(iterAnchoredPhysicals()); }
	inline ConstCustomPartIteratorFactory<T> iterAnchoredParts() const { return ConstCustomPartIteratorFactory<T>(iterAnchoredPhysicals()); }

	inline CustomPartIteratorFactory<T> iterFreeParts() { return CustomPartIteratorFactory<T>(iterFreeParts()); }
	inline ConstCustomPartIteratorFactory<T> iterFreeParts() const { return ConstCustomPartIteratorFactory<T>(iterFreeParts()); }
};

double computeCombinedInertiaBetween(const Physical& first, const Physical& second, const Vec3& localColissionFirst, const Vec3& localColissionSecond, const Vec3& colissionNormal);
