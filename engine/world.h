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

#define FREE_PARTS 0x1
#define TERRAIN_PARTS 0x2
#define ALL_PARTS FREE_PARTS | TERRAIN_PARTS


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
using DoubleFilterIter = FilteredIterator<IteratorGroup<TreeIterFactory<Part, Filter>, 2>, IteratorEnd, Filter>;

template<typename Filter>
using ConstDoubleFilterIter = FilteredIterator<IteratorGroup<TreeIterFactory<const Part, Filter>, 2>, IteratorEnd, Filter>;



/*template<typename Filter>
class FilteredWorldIterator : DoubleFilterIter<Filter> {
public:
	void add(BoundsTree<Part>& tree) {
		DoubleFilterIter<Filter>::iter.add(tree.iterFiltered(DoubleFilterIter<Filter>::filter));
	}
	FilteredWorldIterator(const Filter& filter) : DoubleFilterIter<Filter>(IteratorGroup<TreeIterFactory<Part, Filter>, 3>(), IteratorEnd(), filter) {}
};*/

class WorldPrototype {
private:
	friend class Physical;
	friend class Part;

	std::queue<std::function<void(WorldPrototype&)>> waitingOperations;

	size_t getTotalVertexCount();
	void processQueue();

	

	void addPartUnsafe(Part* p, bool anchored);
	void removePartUnsafe(Part* p);
	void removePhysicalUnsafe(Physical* p);
	void attachPartUnsafe(Part* part, Physical& phys, CFrame attachment);
	void detachPartUnsafe(Part* part);
	void pushOperation(const std::function<void(WorldPrototype&)>& operation);
	
	void setPartCFrame(Part* part, const GlobalCFrame& newCFrame);
	void updatePartBounds(const Part* updatedPart, const Bounds& oldBounds);
	void updatePartGroupBounds(const Part* mainPart, const Bounds& oldMainPartBounds);

public:
	mutable std::shared_mutex lock;
	mutable std::mutex queueLock;
	BoundsTree<Part> objectTree;
	BoundsTree<Part> terrainTree;

	std::vector<ConstraintGroup> constraints;

	size_t age = 0;

	SplitUnorderedList<Physical*> physicals;

	WorldPrototype();
	WorldPrototype(size_t initialPartCapacity);

	WorldPrototype(const WorldPrototype&) = delete;
	WorldPrototype(WorldPrototype&&) = delete;
	WorldPrototype& operator=(const WorldPrototype&) = delete;
	WorldPrototype& operator=(WorldPrototype&&) = delete;

	void tick(double deltaT);

	void requestModification(const std::function<void(WorldPrototype&)>& function);

	void addPart(Part* part, bool anchored = false);
	void attachPart(Part* part, Physical& phys, CFrame attachment);
	void detachPart(Part* part);
	void removePart(Part* part);
	void removePhysical(Physical* part);

	void addTerrainPart(Part* part);
	void removeTerrainPart(Part* part);
	void optimizeTerrain();

	inline bool isAnchored(Physical* p) const {
		return p->anchored;
	}

	inline void anchor(Physical* p) {
		p->setAnchored(true);
	}

	inline void unanchor(Physical* p) {
		p->setAnchored(false);
	}

	inline size_t getAnchoredPartCount() const {
		return terrainTree.getNumberOfObjects();
	}

	inline size_t getFreePartCount() const {
		return objectTree.getNumberOfObjects();
	}

	inline size_t getPartCount() const {
		return getAnchoredPartCount() + getFreePartCount();
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
	IteratorFactoryWithEnd<DoubleFilterIter<Filter>> iterPartsFiltered(const Filter& filter, unsigned int partsMask = ALL_PARTS) {

		size_t size = 0;
		TreeIterFactory<Part, Filter> iters[2];
		if (partsMask & FREE_PARTS) {
			iters[size++] = objectTree.iterFiltered(filter);
		}
		if (partsMask & TERRAIN_PARTS) {
			iters[size++] = terrainTree.iterFiltered(filter);
		}

		IteratorGroup<TreeIterFactory<Part, Filter>, 2> group(iters, size);

		DoubleFilterIter<Filter> doubleFilter(group, IteratorEnd(), filter);
		
		return IteratorFactoryWithEnd<DoubleFilterIter<Filter>>(std::move(doubleFilter));
	}

	template<typename Filter>
	IteratorFactoryWithEnd<ConstDoubleFilterIter<Filter>> iterPartsFiltered(const Filter& filter, unsigned int partsMask = ALL_PARTS) const {

		size_t size = 0;
		TreeIterFactory<Part, Filter> iters[2];
		if (partsMask & FREE_PARTS) {
			iters[size++] = objectTree.iterFiltered(filter);
		}
		if (partsMask & TERRAIN_PARTS) {
			iters[size++] = terrainTree.iterFiltered(filter);
		}

		IteratorGroup<TreeIterFactory<Part, Filter>, 2> group(iters, size);

		ConstDoubleFilterIter<Filter> doubleFilter(group, IteratorEnd(), filter);

		return IteratorFactoryWithEnd<ConstDoubleFilterIter<Filter>>(std::move(doubleFilter));
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
	IteratorFactoryWithEnd<CastingIterator<DoubleFilterIter<Filter>, T&>> iterPartsFiltered(const Filter& filter, unsigned int partsMask = FREE_PARTS) {
		return IteratorFactoryWithEnd<CastingIterator<DoubleFilterIter<Filter>, T&>>(
			CastingIterator<DoubleFilterIter<Filter>, T&>(
				WorldPrototype::iterPartsFiltered(filter, partsMask).begin()
			)
		);
	}

	template<typename Filter>
	IteratorFactoryWithEnd<CastingIterator<ConstDoubleFilterIter<Filter>, const T&>> iterPartsFiltered(const Filter& filter, unsigned int partsMask = FREE_PARTS) const {
		return IteratorFactoryWithEnd<CastingIterator<ConstDoubleFilterIter<Filter>, const T&>>(
			CastingIterator<ConstDoubleFilterIter<Filter>, const T&>(
				WorldPrototype::iterPartsFiltered(filter, partsMask).begin()
			)
		);
	}
};
