#pragma once

#include "../util/resource/resource.h"

#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/visualShape.h"

namespace P3D::Engine {

class MeshResource;

class MeshAllocator : public ResourceAllocator<MeshResource> {
public:
	virtual MeshResource* load(const std::string& name, const std::string& path) override;
};

class MeshResource : public Resource {
private:
	Graphics::IndexedMesh* mesh;
	Graphics::VisualShape shape;
public:
	DEFINE_RESOURCE(Mesh, "../res/fonts/default/default.ttf");

	MeshResource(const std::string& path, Graphics::IndexedMesh* mesh, Graphics::VisualShape shape) : Resource(path, path), mesh(mesh), shape(shape) {

	}

	MeshResource(const std::string& name, const std::string& path, Graphics::IndexedMesh* mesh, Graphics::VisualShape shape) : Resource(name, path), mesh(mesh), shape(shape) {

	}

	Graphics::IndexedMesh* getMesh() {
		return mesh;
	};

	Graphics::VisualShape getShape() {
		return shape;
	}

	virtual void close() override {
		mesh->close();
	}

	static MeshAllocator getAllocator() {
		return MeshAllocator();
	}
};

};