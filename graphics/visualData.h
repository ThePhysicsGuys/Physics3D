#pragma once

namespace P3D {

struct VisualData {

	// The mesh id in the mesh registry
	std::size_t id;

	// Whether the mesh has UV coordinates
	bool hasUVs;

	// Whether the mesh has normal vectors
	bool hasNormals;

	// The render mode, default is fill
	int mode = 0x1B02;
};

}