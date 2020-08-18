#pragma once

#include "datastructures/boundsTree.h"
#include "part.h"
#include "layerRef.h"

class WorldLayer {
public:
	// objectTree, terrainTree
	BoundsTree<Part> trees[2];

	inline BoundsTree<Part>& getObjectTree() { return trees[0]; }
	inline const BoundsTree<Part>& getObjectTree() const { return trees[0]; }

	inline BoundsTree<Part>& getTerrainTree() { return trees[1]; }
	inline const BoundsTree<Part>& getTerrainTree() const { return trees[1]; }

	static inline BoundsTree<Part>& getTree(LayerRef ref) {
		return ref.layer->trees[static_cast<int>(ref.subLayer)];
	}
};

