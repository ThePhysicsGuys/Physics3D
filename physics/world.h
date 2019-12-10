#pragma once

#include <vector>

#include "part.h"
#include "physical.h"
#include "constraintGroup.h"
#include "datastructures/iterators.h"
#include "datastructures/iteratorEnd.h"
#include "datastructures/boundsTree.h"
#include "math/linalg/largeMatrix.h"

#define FREE_PARTS 0x1
#define TERRAIN_PARTS 0x2
#define ALL_PARTS FREE_PARTS | TERRAIN_PARTS

struct Colission {
	Part* p1;
	Part* p2;
	Position intersection;
	Vec3 exitVector;
};

class ExternalForce;

template<typename Filter>
using DoubleFilterIter = FilteredIterator<IteratorGroup<TreeIterFactory<Part, Filter>, 2>, IteratorEnd, Filter>;

template<typename Filter>
using ConstDoubleFilterIter = FilteredIterator<IteratorGroup<TreeIterFactory<const Part, Filter>, 2>, IteratorEnd, Filter>;

using WorldPartIter = IteratorGroup<IteratorFactoryWithEnd<BoundsTreeIter<TreeIterator, Part>>, 2>;
using ConstWorldPartIter = IteratorGroup<IteratorFactoryWithEnd<BoundsTreeIter<ConstTreeIterator, const Part>>, 2>;

class Layer;

class WorldPrototype {
private:
	friend class Physical;
	friend class Part;

	std::vector<ExternalForce*> externalForces;

	std::vector<Colission> currentObjectColissions;
	std::vector<Colission> currentTerrainColissions;

	void setPartCFrame(Part* part, const GlobalCFrame& newCFrame);
	void updatePartBounds(const Part* updatedPart, const Bounds& oldBounds);
	void updatePartGroupBounds(const Part* mainPart, const Bounds& oldMainPartBounds);
	void removePartFromTrees(const Part* part);

	BoundsTree<Part>& getTreeForPart(const Part* part);
	const BoundsTree<Part>& getTreeForPart(const Part* part) const;

	std::vector<Layer> layers;
	// signifies which layers collide
	LargeSymmetricMatrix<bool> colissionMatrix;

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
	size_t objectCount = 0;
	double deltaT;

	std::vector<MotorizedPhysical*> physicals;

	WorldPrototype(double deltaT);
	~WorldPrototype();

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

	// creates a new layer, returning it's ID
	int createLayer();

	inline size_t getPartCount(int partsMask = ALL_PARTS) const {
		return objectCount;
	}

	virtual double getTotalKineticEnergy() const;
	virtual double getTotalPotentialEnergy() const;
	virtual double getPotentialEnergyOfPhysical(const Physical& p) const;
	virtual double getTotalEnergy() const;

	void addExternalForce(ExternalForce* force);
	void removeExternalForce(ExternalForce* force);


	virtual bool isValid() const;

	IteratorFactory<std::vector<MotorizedPhysical*>::iterator> iterPhysicals() { return IteratorFactory<std::vector<MotorizedPhysical*>::iterator>(physicals.begin(), physicals.end()); }
	IteratorFactory<std::vector<MotorizedPhysical*>::const_iterator> iterPhysicals() const { return IteratorFactory<std::vector<MotorizedPhysical*>::const_iterator>(physicals.begin(), physicals.end()); }

	template<typename Filter>
	IteratorFactoryWithEnd<DoubleFilterIter<Filter>> iterPartsFiltered(const Filter& filter, int partsMask = ALL_PARTS) {

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
	IteratorFactoryWithEnd<ConstDoubleFilterIter<Filter>> iterPartsFiltered(const Filter& filter, int partsMask = ALL_PARTS) const {

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

	IteratorFactoryWithEnd<WorldPartIter> iterParts(int partsMask = ALL_PARTS);
	IteratorFactoryWithEnd<ConstWorldPartIter> iterParts(int partsMask = ALL_PARTS) const;
};

class ExternalForce {
public:
	virtual void apply(WorldPrototype* world) = 0;
	virtual double getPotentialEnergyForObject(const WorldPrototype* world, const Part&) const = 0;
	virtual double getPotentialEnergyForObject(const WorldPrototype* world, const Physical& phys) const {
		double total = 0.0;
		for (const Part& p : phys) {
			total += this->getPotentialEnergyForObject(world, p);
		}
		return total;
	}
	virtual double getTotalPotentialEnergyForThisForce(const WorldPrototype* world) const {
		double total = 0.0;
		for (MotorizedPhysical* p : world->iterPhysicals()) {
			total += this->getPotentialEnergyForObject(world, *p);
		}
		return total;
	}
};

template<typename T = Part>
class World : public WorldPrototype {
public:
	World(double deltaT) : WorldPrototype(deltaT) {}

	template<typename Filter>
	IteratorFactoryWithEnd<CastingIterator<DoubleFilterIter<Filter>, T&>> iterPartsFiltered(const Filter& filter, int partsMask = ALL_PARTS) {
		return IteratorFactoryWithEnd<CastingIterator<DoubleFilterIter<Filter>, T&>>(
			CastingIterator<DoubleFilterIter<Filter>, T&>(
				WorldPrototype::iterPartsFiltered(filter, partsMask).begin()
			)
		);
	}

	template<typename Filter>
	IteratorFactoryWithEnd<CastingIterator<ConstDoubleFilterIter<Filter>, const T&>> iterPartsFiltered(const Filter& filter, int partsMask = ALL_PARTS) const {
		return IteratorFactoryWithEnd<CastingIterator<ConstDoubleFilterIter<Filter>, const T&>>(
			CastingIterator<ConstDoubleFilterIter<Filter>, const T&>(
				WorldPrototype::iterPartsFiltered(filter, partsMask).begin()
			)
		);
	}

	IteratorFactoryWithEnd<CastingIterator<WorldPartIter, T&>> iterParts(int partsMask = ALL_PARTS) {
		return IteratorFactoryWithEnd<CastingIterator<WorldPartIter, T&>>(
			CastingIterator<WorldPartIter, T&>(
				WorldPrototype::iterParts(partsMask).begin()
			)
		);
	}

	IteratorFactoryWithEnd<CastingIterator<ConstWorldPartIter, const T&>> iterParts(int partsMask = ALL_PARTS) const {
		return IteratorFactoryWithEnd<CastingIterator<ConstWorldPartIter, const T&>>(
			CastingIterator<ConstWorldPartIter, const T&>(
				WorldPrototype::iterParts(partsMask).begin()
			)
		);
	}
};
