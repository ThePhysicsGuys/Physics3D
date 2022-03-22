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
	Graphics::ExtendedTriangleMesh shape;
public:
	DEFINE_RESOURCE(Mesh, "../res/fonts/default/default.ttf");

	MeshResource(const std::string& path, Graphics::ExtendedTriangleMesh shape) : Resource(path, path), shape(shape) {

	}

	MeshResource(const std::string& name, const std::string& path, Graphics::ExtendedTriangleMesh shape) : Resource(name, path), shape(shape) {

	}

	Graphics::ExtendedTriangleMesh* getShape() {
		return &shape;
	}

	void close() override {
		
	}

	static MeshAllocator getAllocator() {
		return MeshAllocator();
	}
};

};