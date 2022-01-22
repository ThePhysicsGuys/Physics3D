#include "core.h"

#include "meshResource.h"
#include "../io/import.h"
#include "../graphics/extendedTriangleMesh.h"

namespace P3D::Engine {

MeshResource* MeshAllocator::load(const std::string& name, const std::string& path) {
	Graphics::ExtendedTriangleMesh shape = OBJImport::load(path);
	Graphics::IndexedMesh* mesh = new Graphics::IndexedMesh(shape);
	return new MeshResource(name, path, mesh, shape);
}

};