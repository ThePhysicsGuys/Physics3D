#pragma once

#include <vector>

#include "part.h"
#include "physical.h"
#include "constraintGroup.h"
#include "datastructures/iterators.h"
#include "datastructures/iteratorEnd.h"
#include "datastructures/boundsTree.h"

#define FREE_PARTS 0x1
#define TERRAIN_PARTS 0x2
#define ALL_PARTS FREE_PARTS | TERRAIN_PARTS

struct Colission {
	Part* p1;
	Part* p2;
	Position intersection;
	Vec3 exitVector;
};

typedef DereferencingIterator<std::vector<Physical*>::iterator> PhysicalIter;
typedef DereferencingIterator<std::vector<Physical*>::const_iterator> ConstPhysicalIter;

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

class WorldPrototype {
private:
	friend class Physical;
	friend class Part;

	std::vector<Colission> currentObjectColissions;
	std::vector<Colission> currentTerrainColissions;

	void setPartCFrame(Part* part, const GlobalCFrame& newCFrame);
	void updatePartBounds(const Part* updatedPart, const Bounds& oldBounds);
	void updatePartGroupBounds(const Part* mainPart, const Bounds& oldMainPartBounds);

	BoundsTree<Part>& getTreeForType(PartClass pc);
	const BoundsTree<Part>& getTreeForType(PartClass pc) const;

protected:
	virtual void applyExternalForces();
	virtual void findColissions();
	virtual void handleColissions();
	virtual void handleConstraints();
	virtual void update();

public:
	BoundsTree<Part> objectTree;
	BoundsTree<Part> terrainTree;

	std::vector<ConstraintGroup> constraints;

	size_t age = 0;
	double deltaT;

	std::vector<Physical*> physicals;

	WorldPrototype(double deltaT);

	WorldPrototype(const WorldPrototype&) = delete;
	WorldPrototype(WorldPrototype&&) = delete;
	WorldPrototype& operator=(const WorldPrototype&) = delete;
	WorldPrototype& operator=(WorldPrototype&&) = delete;

	virtual void tick();

	void addPart(Part* part, bool anchored = false);
	void removePart(Part* part);
	void removePhysical(Physical* part);

	void addTerrainPart(Part* part);
	void optimizeTerrain();

	inline size_t getTerrainPartCount() const {
		return terrainTree.getNumberOfObjects();
	}

	inline size_t getFreePartCount() const {
		return objectTree.getNumberOfObjects();
	}

	inline size_t getPartCount() const {
		return getTerrainPartCount() + getFreePartCount();
	}

	bool isValid() const;

	virtual double getTotalKineticEnergy() const;
	virtual double getTotalPotentialEnergy() const;
	virtual double getPotentialEnergyOfPhysical(const Physical& p) const;
	virtual double getTotalEnergy() const;


	IteratorFactory<PhysicalIter> iterPhysicals() { return IteratorFactory<PhysicalIter>(physicals.begin(), physicals.end()); }
	IteratorFactory<ConstPhysicalIter> iterPhysicals() const { return IteratorFactory<ConstPhysicalIter>(physicals.begin(), physicals.end()); }

	BasicPartIter begin() { return BasicPartIter(iterPhysicals().begin(), iterPhysicals().end()); }
	ConstBasicPartIter begin() const { return ConstBasicPartIter(iterPhysicals().begin(), iterPhysicals().end()); }
	IteratorEnd end() const { return IteratorEnd(); }

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
	World(double deltaT) : WorldPrototype(deltaT) {}
	inline CastingIterator<BasicPartIter, T&> begin() { return CastingIterator<BasicPartIter, T&>(WorldPrototype::begin()); }
	inline CastingIterator<ConstBasicPartIter, const T&> begin() const { return CastingIterator<ConstBasicPartIter, const T&>(WorldPrototype::begin()); }
	inline IteratorEnd end() const { return IteratorEnd(); }

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
