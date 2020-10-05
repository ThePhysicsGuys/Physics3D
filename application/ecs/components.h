#pragma once

#include <string>
#include "extendedPart.h"
#include "../graphics/mesh/indexedMesh.h"

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

// The model of the entity, as it is being rendered
struct Mesh : RefCountable {
	Graphics::IndexedMesh* mesh;

	Mesh(Graphics::IndexedMesh* mesh) : mesh(mesh) {}

	Graphics::IndexedMesh* operator->() {
		return mesh;
	}
};

}

};