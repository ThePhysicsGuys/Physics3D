#pragma once

#include "../util/resource/resource.h"

#include "../graphics/mesh/indexedMesh.h"

class MeshResource;

class MeshAllocator : public ResourceAllocator<MeshResource> {
public:
	virtual MeshResource* load(const std::string& name, const std::string& path) override;
};

class MeshResource : public Resource, public IndexedMesh {
public:
	DEFINE_RESOURCE(Mesh, "../res/fonts/default/default.ttf");

	MeshResource(const std::string& path, IndexedMesh&& mesh) : Resource(path, path), IndexedMesh(std::move(mesh)) {

	}

	MeshResource(const std::string& name, const std::string& path, IndexedMesh&& mesh) : Resource(name, path), IndexedMesh(std::move(mesh)) {

	}

	virtual void close() override {
		IndexedMesh::close();
	};

	static MeshAllocator getAllocator() {
		return MeshAllocator();
	}
};

