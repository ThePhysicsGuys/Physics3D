#pragma once

#include <vector>

#include "datastructures/boundsTree.h"
#include "part.h"
#include "colissionBuffer.h"

namespace P3D {
class WorldPrototype;
class ColissionLayer;

class WorldLayer {
public:
	ChosenBoundsTree tree;
	ColissionLayer* parent;

	explicit WorldLayer(ColissionLayer* parent);

	WorldLayer(WorldLayer&& other) noexcept;
	WorldLayer& operator=(WorldLayer&& other) noexcept;
	WorldLayer(const WorldLayer&) = delete;
	WorldLayer& operator=(const WorldLayer&) = delete;

	~WorldLayer();

	void refresh();

	void addPart(Part* newPart);
	void removePart(Part* partToRemove);
	void addIntoGroup(Part* newPart, Part* group);
	template<typename PartIterBegin, typename PartIterEnd>
	void addAllToGroup(PartIterBegin begin, PartIterEnd end, Part* group) {
		tree.addAllToGroup(begin, end, group);
	}
	//void addIntoGroup(MotorizedPhysical* newPhys, Part* group);

	void notifyPartBoundsUpdated(const Part* updatedPart, const Bounds& oldBounds);
	void notifyPartGroupBoundsUpdated(const Part* mainPart, const Bounds& oldMainPartBounds);
	/*
		When a part is std::move'd to a different location, this function is called to update any pointers
		This is something that in general should not be performed when the part is already in a world, but this function is provided for completeness
	*/
	void notifyPartStdMoved(Part* oldPartPtr, Part* newPartPtr) noexcept;

	void mergeGroups(Part* first, Part* second);
	void moveIntoGroup(Part* partToMove, Part* group);
	void moveOutOfGroup(Part* part);
	void joinPartsIntoNewGroup(Part* p1, Part* p2);

	template<typename PartIterBegin, typename PartIterEnd>
	void splitGroup(PartIterBegin begin, PartIterEnd end) {
		tree.splitGroup(begin, end);
	}
	void optimize() {
		tree.maxImproveStructure();
	}

	int getID() const;
};

WorldLayer* getLayerByID(std::vector<ColissionLayer>& knownLayers, int id);
const WorldLayer* getLayerByID(const std::vector<ColissionLayer>& knownLayers, int id);
int getMaxLayerID(const std::vector<ColissionLayer>& knownLayers);

class ColissionLayer {
public:
	static constexpr int FREE_PARTS_LAYER = 0;
	static constexpr int TERRAIN_PARTS_LAYER = 1;
	static constexpr int NUMBER_OF_SUBLAYERS = 2;

	WorldLayer subLayers[NUMBER_OF_SUBLAYERS];
	// freePartsLayer
	// terrainLayer
	WorldPrototype* world;
	bool collidesInternally;

	ColissionLayer();
	ColissionLayer(WorldPrototype* world, bool collidesInternally);

	ColissionLayer(ColissionLayer&& other) noexcept;
	ColissionLayer& operator=(ColissionLayer&& other) noexcept;

	void refresh();

	void getInternalColissions(ColissionBuffer& curColissions) const;

	int getID() const;
};
void getColissionsBetween(const ColissionLayer& a, const ColissionLayer& b, ColissionBuffer& curColissions);
};
