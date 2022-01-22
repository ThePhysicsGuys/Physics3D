#pragma once

#include "../util/resource/resource.h"

#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/extendedTriangleMesh.h"

namespace P3D::Engine {

class MeshResource;

class MeshAllocator : public ResourceAllocator<MeshResource> {
public:
	virtual MeshResource* load(const std::string& name, const std::string& path) override;
};

class MeshResource : public Resource {
private:
	Graphics::IndexedMesh* mesh;
	Graphics::ExtendedTriangleMesh shape;
public:
	DEFINE_RESOURCE(Mesh, "../res/fonts/default/default.ttf");

	MeshResource(const std::string& path, Graphics::IndexedMesh* mesh, Graphics::ExtendedTriangleMesh shape) : Resource(path, path), mesh(mesh), shape(shape) {

	}

	MeshResource(const std::string& name, const std::string& path, Graphics::IndexedMesh* mesh, Graphics::ExtendedTriangleMesh shape) : Resource(name, path), mesh(mesh), shape(shape) {

	}

	Graphics::IndexedMesh* getMesh() {
		return mesh;
	};

	Graphics::ExtendedTriangleMesh getShape() {
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