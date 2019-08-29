#pragma once

#include "part.h"
#include "physical.h"
#include <vector>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <functional>
#include "datastructures/splitUnorderedList.h"
#include "datastructures/boundsTree.h"

#include "datastructures/iterators.h"
#include "datastructures/iteratorEnd.h"

#include "constraintGroup.h"

typedef ListOfPtrIter<Physical> PhysicalIter;
typedef ListOfPtrIter<const Physical> ConstPhysicalIter;

typedef IteratorFactory<PhysicalIter> PhysicalIterFactory;
typedef IteratorFactory<ConstPhysicalIter> ConstPhysicalIterFactory;

typedef CompositeIterator<PhysicalIter, PhysicalIter> BasicPartIter;
typedef CompositeIterator<ConstPhysicalIter, ConstPhysicalIter> ConstBasicPartIter;

typedef IteratorFactoryWithEnd<BasicPartIter> BasicPartIterFactory;
typedef IteratorFactoryWithEnd<ConstBasicPartIter> ConstBasicPartIterFactory;

template<typename ExtendedPart>
using BasicExtendedPartIterFactory = CastingIteratorFactoryWithEnd<BasicPartIterFactory, ExtendedPart&>;
template<typename ExtendedPart>
using ConstBasicExtendedPartIterFactory = CastingIteratorFactoryWithEnd<ConstBasicPartIterFactory, const ExtendedPart&>;

template<typename Filter>
using DoubleFilterIter = FilteredIterator<FilteredBoundsTreeIter<Filter, Part>, IteratorEnd, Filter>;

class WorldPrototype {
private:
	std::queue<std::function<void(WorldPrototype*)>> waitingOperations;

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
	BoundsTree<Part> objectTree;
	std::vector<ConstraintGroup> constraints;

	size_t age = 0;

	SplitUnorderedList<Physical*> physicals;

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
		return p->anchored;
	}

	inline void anchor(Physical* p) {
		p->setAnchored(true);
	}

	inline void unanchor(Physical* p) {
		p->setAnchored(false);
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


	PhysicalIterFactory iterPhysicals() { return PhysicalIterFactory(physicals.begin(), physicals.end()); }
	ConstPhysicalIterFactory iterPhysicals() const { return ConstPhysicalIterFactory(physicals.begin(), physicals.end()); }

	PhysicalIterFactory iterAnchoredPhysicals() { return PhysicalIterFactory(physicals.begin(), physicals.getSplitOffset()); }
	ConstPhysicalIterFactory iterAnchoredPhysicals() const { return ConstPhysicalIterFactory(physicals.begin(), physicals.getSplitOffset()); }

	PhysicalIterFactory iterFreePhysicals() { return PhysicalIterFactory(physicals.getSplitOffset(), physicals.end()); }
	ConstPhysicalIterFactory iterFreePhysicals() const { return ConstPhysicalIterFactory(physicals.getSplitOffset(), physicals.end()); }

	BasicPartIter begin() { return BasicPartIter(iterPhysicals().begin(), iterPhysicals().end()); }
	ConstBasicPartIter begin() const { return ConstBasicPartIter(iterPhysicals().begin(), iterPhysicals().end()); }
	IteratorEnd end() const { return IteratorEnd(); }

	BasicPartIterFactory iterAnchoredParts() { return BasicPartIterFactory(BasicPartIter(iterAnchoredPhysicals().begin(), iterAnchoredPhysicals().end())); }
	ConstBasicPartIterFactory iterAnchoredParts() const { return ConstBasicPartIterFactory(ConstBasicPartIter(iterAnchoredPhysicals().begin(), iterAnchoredPhysicals().end())); }

	BasicPartIterFactory iterFreeParts() { return BasicPartIterFactory(BasicPartIter(iterFreePhysicals().begin(), iterFreePhysicals().end())); }
	ConstBasicPartIterFactory iterFreeParts() const { return ConstBasicPartIterFactory(ConstBasicPartIter(iterFreePhysicals().begin(), iterFreePhysicals().end())); }

	template<typename Filter>
	IteratorFactory<CastingIterator<DoubleFilterIter<Filter>, Part&>, IteratorEnd> iterPartsFiltered(const Filter& filter) {
		return IteratorFactory<CastingIterator<DoubleFilterIter<Filter>, Part&>, IteratorEnd>(
			CastingIterator<DoubleFilterIter<Filter>, Part&>(
				DoubleFilterIter<Filter>(objectTree.iterFiltered(filter).begin(), IteratorEnd(), filter)
				),
			IteratorEnd()
		);
	}
};

template<typename T = Part>
class World : public WorldPrototype {
public:
	inline CastingIterator<BasicPartIter, T&> begin() { return CastingIterator<BasicPartIter, T&>(WorldPrototype::begin()); }
	inline CastingIterator<ConstBasicPartIter, const T&> begin() const { return CastingIterator<ConstBasicPartIter, const T&>(WorldPrototype::begin()); }
	inline IteratorEnd end() const { return IteratorEnd(); }

	inline BasicExtendedPartIterFactory<T> iterAnchoredParts() { return BasicExtendedPartIterFactory<T>(WorldPrototype::iterAnchoredParts()); }
	inline ConstBasicExtendedPartIterFactory<T> iterAnchoredParts() const { return ConstBasicExtendedPartIterFactory<T>(WorldPrototype::iterAnchoredParts()); }

	inline BasicExtendedPartIterFactory<T> iterFreeParts() { return BasicExtendedPartIterFactory<T>(WorldPrototype::iterFreeParts()); }
	inline ConstBasicExtendedPartIterFactory<T> iterFreeParts() const { return ConstBasicExtendedPartIterFactory<T>(WorldPrototype::iterFreeParts()); }

	template<typename Filter>
	IteratorFactory<CastingIterator<DoubleFilterIter<Filter>, T&>, IteratorEnd> iterPartsFiltered(const Filter& filter) {
		return IteratorFactory<CastingIterator<DoubleFilterIter<Filter>, T&>, IteratorEnd>(
			CastingIterator<DoubleFilterIter<Filter>, T&>(
				DoubleFilterIter<Filter>(objectTree.iterFiltered(filter).begin(), IteratorEnd(), filter)
			), 
			IteratorEnd()
		);
	}
};
