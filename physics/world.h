#pragma once

#include <vector>

#include "part.h"
#include "physical.h"
#include "constraintGroup.h"
#include "datastructures/iterators.h"
#include "datastructures/iteratorEnd.h"
#include "layer.h"
#include "softLink.h"
#include "colissionBuffer.h"

#include "springLink.h"
#include <memory>

struct Colission {
	Part* p1;
	Part* p2;
	Position intersection;
	Vec3 exitVector;
};

class ExternalForce;
class WorldLayer;

template<bool IsConst>
class WorldLayerIter {
protected:
	const_if<ColissionLayer, IsConst>* curLayer;
	const_if<ColissionLayer, IsConst>* finalLayer;
	const_if<WorldLayer, IsConst>* curWorldLayer;

public:
	// this ctor creates an iter which is immediately at it's end
	WorldLayerIter() : curLayer(nullptr), finalLayer(nullptr), curWorldLayer(nullptr) {}

	WorldLayerIter(const_if<ColissionLayer, IsConst>* firstLayer, const_if<ColissionLayer, IsConst>* finalLayer) :
		curLayer(firstLayer),
		finalLayer(finalLayer),
		curWorldLayer(curLayer->subLayers) {}

	bool operator!=(IteratorEnd) const {
		return curLayer != finalLayer;
	}
	bool operator==(IteratorEnd) const {
		return curLayer == finalLayer;
	}

	WorldLayerIter& operator++() {
		++curWorldLayer;
		if(curWorldLayer == curLayer->subLayers + ColissionLayer::NUMBER_OF_SUBLAYERS) {
			++curLayer;
			if(curLayer == finalLayer) return *this;
			curWorldLayer = curLayer->subLayers;
		}
		return *this;
	}

	const_if<WorldLayer, IsConst>* operator*() const {
		return curWorldLayer;
	}
};

template<bool IsConst>
class WorldIteratorTemplate {
protected:
	WorldLayerIter<IsConst> worldLayerIter;
	decltype((*worldLayerIter)->tree.begin()) curIter;
	
	inline void checkMoveToNextLayer() {
		while(!(curIter != IteratorEnd())) {
			++worldLayerIter;
			if(worldLayerIter == IteratorEnd()) return;
			curIter = (*worldLayerIter)->tree.begin();
		}
	}

public:
	WorldIteratorTemplate() = default;

	WorldIteratorTemplate(const_if<ColissionLayer, IsConst>* firstLayer, const_if<ColissionLayer, IsConst>* finalLayer) :
		worldLayerIter(firstLayer, finalLayer),
		curIter((*worldLayerIter)->tree.begin()) {

		assert(firstLayer != finalLayer);

		checkMoveToNextLayer();
	}

	WorldIteratorTemplate& operator++() {
		++curIter;
		checkMoveToNextLayer();
		return *this;
	}

	bool operator!=(IteratorEnd) const {
		return this->worldLayerIter != IteratorEnd();
	}

	const_if<Part, IsConst>& operator*() const {
		return *curIter;
	}
};

template<bool IsConst, typename Filter>
class FilteredWorldIteratorTemplate {
protected:
	WorldLayerIter<IsConst> worldLayerIter;
	Filter filter;
	decltype((*worldLayerIter)->tree.iterFiltered(filter).begin()) curIter;

	inline void checkMoveToNextLayer() {
		while(!(curIter != IteratorEnd())) {
			++worldLayerIter;
			if(worldLayerIter == IteratorEnd()) return;
			curIter = (*worldLayerIter)->tree.iterFiltered(this->filter).begin();
		}
	}

public:
	FilteredWorldIteratorTemplate() = default;

	FilteredWorldIteratorTemplate(const_if<ColissionLayer, IsConst>* firstLayer, const_if<ColissionLayer, IsConst>* finalLayer, const Filter& filter) :
		worldLayerIter(firstLayer, finalLayer),
		filter(filter),
		curIter((*worldLayerIter)->tree.iterFiltered(this->filter).begin()) {

		assert(firstLayer != finalLayer);

		checkMoveToNextLayer();
	}

	FilteredWorldIteratorTemplate& operator++() {
		++curIter;
		checkMoveToNextLayer();
		return *this;
	}

	bool operator!=(IteratorEnd) const {
		return this->worldLayerIter != IteratorEnd();
	}

	const_if<Part, IsConst>& operator*() const {
		return *curIter;
	}
};

typedef WorldIteratorTemplate<false> WorldIterator;
typedef WorldIteratorTemplate<true> ConstWorldIterator;

template<typename Filter>
using FilteredWorldIterator = FilteredWorldIteratorTemplate<false, Filter>;
template<typename Filter>
using FilteredConstWorldIterator = FilteredWorldIteratorTemplate<true, Filter>;

class WorldPrototype {
private:
	friend class Physical;
	friend class MotorizedPhysical;
	friend class ConnectedPhysical;
	friend class Part;
	friend class WorldLayer;
  friend class ColissionLayer;


	/*
		This method is called by World or Physical when new MotorizedPhysicals are created which need to be added to the list
	*/
	void notifyNewPhysicalCreated(MotorizedPhysical* newPhysical);

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

	// called when the part has already been removed from the world
	virtual void deletePart(Part* partToDelete) const;

public:
	std::vector<ExternalForce*> externalForces;
	std::vector<MotorizedPhysical*> physicals;
	std::vector<ConstraintGroup> constraints;

	std::vector<ColissionLayer> layers;
  std::vector<Colission> currentObjectColissions;
  std::vector<Colission> currentTerrainColissions;

  std::vector<SoftLink*> springLinks;
  
	void addLink(SoftLink* link);

	
	ColissionBuffer curColissions;

	/*
		These lists signify which layers collide
	*/
	std::vector<std::pair<int, int>> colissionMask;
	
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
	void addTerrainPart(Part* part, int layerIndex = 0);
	void removePart(Part* part);

	bool doLayersCollide(int layer1, int layer2) const;
	void setLayersCollide(int layer1, int layer2, bool collide);

	int createLayer(bool collidesInternally, bool collidesWithOthers);
	void deleteLayer(int layerIndex, int layerToMoveTo);


	void addPhysicalWithExistingLayers(MotorizedPhysical* motorPhys);

	void optimizeLayers();

	// removes everything from this world, parts, physicals, forces, constraints
	void clear();

	inline size_t getPartCount() const {
		return objectCount;
	}

	size_t getLayerCount() const;

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
	IteratorFactoryWithEnd<FilteredWorldIterator<Filter>> iterPartsFiltered(const Filter& filter) {
		if(layers.size() > 0) {
			ColissionLayer* fst = &layers[0];
			return IteratorFactoryWithEnd<FilteredWorldIterator<Filter>>(FilteredWorldIterator<Filter>(fst, fst + layers.size(), filter));
		} else {
			return IteratorFactoryWithEnd<FilteredWorldIterator<Filter>>(FilteredWorldIterator<Filter>());
		}
	}

	template<typename Filter>
	IteratorFactoryWithEnd<FilteredConstWorldIterator<Filter>> iterPartsFiltered(const Filter& filter) const {
		if(layers.size() > 0) {
			const ColissionLayer* fst = &layers[0];
			return IteratorFactoryWithEnd<FilteredConstWorldIterator<Filter>>(FilteredConstWorldIterator<Filter>(fst, fst + layers.size(), filter));
		} else {
			return IteratorFactoryWithEnd<FilteredConstWorldIterator<Filter>>(FilteredConstWorldIterator<Filter>());
		}
	}

	inline IteratorFactoryWithEnd<WorldIterator> iterParts() {
		if(layers.size() > 0) {
			ColissionLayer* fst = &layers[0];
			return IteratorFactoryWithEnd<WorldIterator>(WorldIterator(fst, fst + layers.size()));
		} else {
			return IteratorFactoryWithEnd<WorldIterator>(WorldIterator());
		}
	}
	inline IteratorFactoryWithEnd<ConstWorldIterator> iterParts() const {
		if(layers.size() > 0) {
			const ColissionLayer* fst = &layers[0];
			return IteratorFactoryWithEnd<ConstWorldIterator>(ConstWorldIterator(fst, fst + layers.size()));
		} else {
			return IteratorFactoryWithEnd<ConstWorldIterator>(ConstWorldIterator());
		}
	}
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
	IteratorFactoryWithEnd<CastingIterator<FilteredWorldIterator<Filter>, T&>> iterPartsFiltered(const Filter& filter) {
		return IteratorFactoryWithEnd<CastingIterator<FilteredWorldIterator<Filter>, T&>>(
			CastingIterator<FilteredWorldIterator<Filter>, T&>(
				WorldPrototype::iterPartsFiltered(filter).begin()
			)
		);
	}

	template<typename Filter>
	IteratorFactoryWithEnd<CastingIterator<FilteredConstWorldIterator<Filter>, const T&>> iterPartsFiltered(const Filter& filter) const {
		return IteratorFactoryWithEnd<CastingIterator<FilteredConstWorldIterator<Filter>, const T&>>(
			CastingIterator<FilteredConstWorldIterator<Filter>, const T&>(
				WorldPrototype::iterPartsFiltered(filter).begin()
			)
		);
	}

	IteratorFactoryWithEnd<CastingIterator<WorldIterator, T&>> iterParts() {
		return IteratorFactoryWithEnd<CastingIterator<WorldIterator, T&>>(
			CastingIterator<WorldIterator, T&>(
				WorldPrototype::iterParts().begin()
			)
		);
	}

	IteratorFactoryWithEnd<CastingIterator<ConstWorldIterator, const T&>> iterParts() const {
		return IteratorFactoryWithEnd<CastingIterator<ConstWorldIterator, const T&>>(
			CastingIterator<ConstWorldIterator, const T&>(
				WorldPrototype::iterParts().begin()
			)
		);
	}

	virtual void onPartAdded(T* part) {}
	virtual void onPartRemoved(T* part) {}
	virtual void deletePart(T* part) const {
		delete part;
	}
	virtual void onPartAdded(Part* part) final override {
		this->onPartAdded(static_cast<T*>(part));
	}
	virtual void onPartRemoved(Part* part) final override {
		this->onPartRemoved(static_cast<T*>(part));
	}
	// called when the part has already been removed from the world
	virtual void deletePart(Part* partToDelete) const final override {
		this->deletePart(static_cast<T*>(partToDelete));
	}
};
