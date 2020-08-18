#pragma once

namespace P3D {

struct VisualData {
	int drawMeshId;
	bool includeUVs;
	bool includeNormals;

	void render(int renderMode) const;
};

}