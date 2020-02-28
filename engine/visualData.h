#pragma once

struct VisualData {
	int drawMeshId;
	bool includeUVs;
	bool includeNormals;

	void render(int renderMode) const;
};
