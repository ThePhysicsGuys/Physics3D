#pragma once

#include <string>
#include "extendedPart.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../engine/visualData.h"

namespace P3D::Application {

namespace Comp {

// The name of an entity
struct Tag : RefCountable {
	std::string name;

	Tag(const std::string& name) : name(name) {}
};

// The model of the entity, as it is being physicsed in the engine 
struct Model : RefCountable {
	ExtendedPart* part;

	Model(ExtendedPart* part) : part(part) {}

	ExtendedPart* operator->() {
		return part;
	}
};

// The mesh of an entity, as it is rendered
struct Mesh : public RefCountable {
	// The render mode, default is fill
	int mode = 0x1B02;

	// The mesh id in the mesh registry
	int id;

	// Whether the mesh has UV coordinates
	bool hasUVs;

	// Whether the mesh has normal vectors
	bool hasNormals;

	Mesh(const VisualData& data) : mode(data.mode), id(data.id), hasUVs(data.hasUVs), hasNormals(data.hasNormals) {}
	Mesh(int mode, int id, bool hasUVs, bool hasNormals) : mode(mode), id(id), hasUVs(hasUVs), hasNormals(hasNormals) {}
	Mesh(int id, bool hasUVs, bool hasNormals) : id(id), hasUVs(hasUVs), hasNormals(hasNormals) {}
};

}

};