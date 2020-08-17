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
class WorldLayer;

template<typename Filter>
using DoubleFilterIter = FilteredIterator<IteratorGroup<TreeIterFactory<Part, Filter>, 2>, IteratorEnd, Filter>;

template<typename Filter>
using ConstDoubleFilterIter = FilteredIterator<IteratorGroup<TreeIterFactory<const Part, Filter>, 2>, IteratorEnd, Filter>;

using WorldPartIter = IteratorGroup<IteratorFactoryWithEnd<BoundsTreeIter<TreeIterator, Part>>, 2>;
using ConstWorldPartIter = IteratorGroup<IteratorFactoryWithEnd<BoundsTreeIter<ConstTreeIterator, const Part>>, 2>;

class WorldPrototype {
private:
	friend class Physical;
	friend class MotorizedPhysical;
	friend class ConnectedPhysical;
	friend class Part;

	std::vector<Colission> currentObjectColissions;
	std::vector<Colission> currentTerrainColissions;

	/*
		This method is called by World or Physical when new MotorizedPhysicals are created which need to be added to the list
	*/
	void notifyNewPhysicalCreatedWhenSplitting(MotorizedPhysical* newPhysical);

	/*
		Splits newlySplitPhysical from mainPhysical in the world tree, also adds the new physical to the list of physicals
	*/
	void notifyPhysicalHasBeenSplit(const MotorizedPhysical* mainPhysical, MotorizedPhysical* newlySplitPhysical);

	/*
		Merges the trees for two physicals. 
		firstPhysical must be part of this world, 
		secondPhysical may or may not be in the world, but is not allowed to be in a different world
	*/
	void notifyPhysicalsMerged(const MotorizedPhysical* firstPhysical, MotorizedPhysical* secondPhysical);
	/*
		Adds a new part that wasn't previously in the tree to the group of the given physical
	*/
	void notifyNewPartAddedToPhysical(const MotorizedPhysical* physical, Part* newPart);

	/*
		Called when a MainPhysical has become obsolete
		This usually happens right before the physical is deleted
	*/
	void notifyMainPhysicalObsolete(MotorizedPhysical* part);

	/*
		Called when a part gets detached from a physical
	*/
	void notifyPartDetachedFromPhysical(Part* part);
	/*
		Called when a part gets removed from a physical
	*/
	void notifyPartRemovedFromPhysical(Part* part);


private: // actually private fields and methods, not to be used by any friends
	void mergePhysicalGroups(const MotorizedPhysical* first, MotorizedPhysical* second);

	std::vector<WorldLayer> layers;
	/*
		Signifies which layers collide
	*/
	LargeSymmetricMatrix<bool> colissionMatrix;

protected:
	// World tick steps
	virtual void applyExternalForces();
	virtual void findColissions();
	virtual void handleColissions();
	virtual void handleConstraints();
	virtual void update();


	// event handlers
	virtual void onPartAdded(Part* newPart);
	virtual void onPartRemoved(Part* removedPart); 

public:
	std::vector<ExternalForce*> externalForces;
	std::vector<MotorizedPhysical*> physicals;
	std::vector<ConstraintGroup> constraints;

	BoundsTree<Part>& objectTree;
	BoundsTree<Part>& terrainTree;

	size_t age = 0;
	size_t objectCount = 0;
	double deltaT;


	WorldPrototype(double deltaT);
	~WorldPrototype();

	WorldPrototype(const WorldPrototype&) = delete;
	WorldPrototype& operator=(const WorldPrototype&) = delete;
	WorldPrototype(WorldPrototype&&) = delete;
	WorldPrototype& operator=(WorldPrototype&&) = delete;


	virtual void tick();

	void addPart(Part* part, int layerIndex = 0);
	void removePart(Part* part);

	void addTerrainPart(Part* part, int layerIndex = 0);
	void optimizeTerrain();

	// removes everything from this world, parts, physicals, forces, constraints
	void clear();

	inline size_t getPartCount(int partsMask = ALL_PARTS) const {
		return objectCount;
	}

	virtual double getTotalKineticEnergy() const;
	virtual double getTotalPotentialEnergy() const;
	virtual double getPotentialEnergyOfPhysical(const MotorizedPhysical& p) const;
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
	virtual double getPotentialEnergyForObject(const WorldPrototype* world, const MotorizedPhysical& phys) const {
		double total = 0.0;
		for (const Part& p : phys.rigidBody) {
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

	virtual void onPartAdded(T* part) {}
	virtual void onPartRemoved(T* part) {}
	virtual void onPartAdded(Part* part) final override {
		this->onPartAdded(static_cast<T*>(part));
	}
	virtual void onPartRemoved(Part* part) final override {
		this->onPartRemoved(static_cast<T*>(part));
	}
};
