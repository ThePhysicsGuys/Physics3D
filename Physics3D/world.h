#pragma once

#include <vector>
#include <mutex>
#include <memory>

#include "part.h"
#include "physical.h"
#include "constraints/constraintGroup.h"
#include "softlinks/softLink.h"
#include "externalforces/externalForce.h"
#include "colissionBuffer.h"

namespace P3D {
class Physical;
class MotorizedPhysical;
class ConnectedPhysical;
class Part;
class WorldLayer;
class ColissionLayer;
class ThreadPool;

class WorldPrototype {
private:
	friend class Physical;
	friend class MotorizedPhysical;
	friend class ConnectedPhysical;
	friend class Part;
	friend class WorldLayer;
	friend class ColissionLayer;

	/*
		Splits newlySplitPhysical from mainPhysical in the world tree, also adds the new physical to the list of physicals
	*/
	void notifyPhysicalHasBeenSplit(const MotorizedPhysical* mainPhysical, MotorizedPhysical* newlySplitPhysical);

protected:


	// event handlers
	virtual void onPartAdded(Part* newPart);
	virtual void onPartRemoved(Part* removedPart);

	// called when the part has already been removed from the world
	virtual void deletePart(Part* partToDelete) const;

public:
	// AABB trees for storing parts
	std::vector<ColissionLayer> layers;
	// Sparse matrix listing layer-layer colissions
	std::vector<std::pair<int, int>> colissionMask;
	
	// All physicals
	std::vector<MotorizedPhysical*> physicals;

	void addPhysicalWithExistingLayers(MotorizedPhysical* motorPhys);

	// Extra world features
	std::vector<ExternalForce*> externalForces;
	std::vector<ConstraintGroup> constraints;
	std::vector<SoftLink*> softLinks;

	void addLink(SoftLink* link);

	ColissionBuffer curColissions;
	size_t age = 0;
	size_t objectCount = 0;
	double deltaT;


	WorldPrototype(double deltaT);
	~WorldPrototype();

	WorldPrototype(const WorldPrototype&) = delete;
	WorldPrototype& operator=(const WorldPrototype&) = delete;
	WorldPrototype(WorldPrototype&&) = delete;
	WorldPrototype& operator=(WorldPrototype&&) = delete;

	virtual void tick(ThreadPool& threadPool);
	void tick();

	virtual void addPart(Part* part, int layerIndex = 0);
	virtual void removePart(Part* part);
	void addTerrainPart(Part* part, int layerIndex = 0);

	bool doLayersCollide(int layer1, int layer2) const;
	void setLayersCollide(int layer1, int layer2, bool collide);

	int createLayer(bool collidesInternally, bool collidesWithOthers);

	// deletes a layer, moves all parts from the deleted layer to layerToMoveTo
	void deleteLayer(int layerIndex, int layerToMoveTo);

	void optimizeLayers();

	// removes everything from this world, parts, physicals, forces, constraints
	void clear();

	inline size_t getPartCount() const {
		return objectCount;
	}

	int getLayerCount() const;

	virtual double getTotalKineticEnergy() const;
	virtual double getTotalPotentialEnergy() const;
	virtual double getPotentialEnergyOfPhysical(const MotorizedPhysical& p) const;
	virtual double getTotalEnergy() const;

	void addExternalForce(ExternalForce* force);
	void removeExternalForce(ExternalForce* force);

	virtual bool isValid() const;

	// include worldIteration.h to use
	// expects a function of the form void(Part& part)
	template<typename Func>
	void forEachPart(const Func& funcToRun) const;

	// include worldIteration.h to use
	// expects a function of the form void(Part& part)
	template<typename Func, typename Filter>
	void forEachPartFiltered(const Filter& filter, const Func& funcToRun) const;
};

template<typename T = Part>
class World : public WorldPrototype {
public:
	World(double deltaT) : WorldPrototype(deltaT) {}

	virtual void onCollide(Part* partA, Part* partB) {}
	virtual void onCollide(T* partA, T* partB) {}

	virtual void onPartAdded(T* part) {}
	virtual void onPartRemoved(T* part) {}

	virtual void deletePart(T* part) const {
		delete part;
	}
	void onPartAdded(Part* part) final override {
		this->onPartAdded(static_cast<T*>(part));
	}
	void onPartRemoved(Part* part) final override {
		this->onPartRemoved(static_cast<T*>(part));
	}
	// called when the part has already been removed from the world
	void deletePart(Part* partToDelete) const final override {
		this->deletePart(static_cast<T*>(partToDelete));
	}

	// include worldIteration.h to use
	// expects a function of the form void(T& part)
	template<typename Func>
	void forEachPart(const Func& funcToRun) const;

	// include worldIteration.h to use
	// expects a function of the form void(T& part)
	template<typename Func, typename Filter>
	void forEachPartFiltered(const Filter& filter, const Func& funcToRun) const;
};
};