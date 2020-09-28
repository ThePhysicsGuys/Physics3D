#pragma once

#include <string>
#include "extendedPart.h"
#include "../graphics/mesh/indexedMesh.h"

namespace P3D::Application {

namespace Comp {

// The name of an entity
struct Tag {
	std::string name;

	Tag(const std::string& name) : name(name) {}
};

// The model of the entity, as it is being physicsed in the engine 
struct Model {
	ExtendedPart* part;

	Model(ExtendedPart* part) : part(part) {}
};

// The model of the entity, as it is being rendered
struct Mesh {
	Graphics::IndexedMesh* mesh;

	Mesh(Graphics::IndexedMesh* mesh) : mesh(mesh) {}
};

// The visual attributes of the mesh
struct Material {

};

}

};