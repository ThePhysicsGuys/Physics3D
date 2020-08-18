#pragma once

#include "math/bounds.h"

class WorldLayer;
class Part;

enum class SubLayer : int {
	OBJECT = 0,
	TERRAIN = 1
};

class LayerRef {
	friend class WorldLayer;
	WorldLayer* layer;
	SubLayer subLayer; //0: objectTree, 1: terrainTree
public:
	inline LayerRef() : layer(nullptr) {}
	inline LayerRef(WorldLayer* layer, SubLayer subLayer) : layer(layer), subLayer(subLayer) {}

	void notifyPartBoundsUpdated(const Part* updatedPart, const Bounds& oldBounds);
	void notifyPartGroupBoundsUpdated(const Part* mainPart, const Bounds& oldMainPartBounds);
	/*
		When a part is std::move'd to a different location, this function is called to update any pointers
		This is something that in general should not be performed when the part is already in a world, but this function is provided for completeness
	*/
	void notifyPartStdMoved(Part* oldPartPtr, Part* newPartPtr) noexcept;

	inline bool operator==(LayerRef other) const {
		return layer == other.layer && subLayer == other.subLayer;
	}
	inline bool operator!=(LayerRef other) const {
		return layer != other.layer || subLayer != other.subLayer;
	}

	inline operator bool() const {
		return layer != nullptr;
	}
};

