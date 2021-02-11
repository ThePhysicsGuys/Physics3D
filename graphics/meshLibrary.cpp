#include "core.h"

#include "meshLibrary.h"

#include <sstream>

#include "meshRegistry.h"
#include "mesh/indexedMesh.h"

namespace P3D::Graphics {

namespace Library {
	
IndexedMesh* cube = nullptr;
IndexedMesh* sphere = nullptr;

void onInit() {
	// Cube
	cube = new IndexedMesh(MeshRegistry::createCube(1));

	// Sphere
	sphere = new IndexedMesh(MeshRegistry::createSphere(1, 3));
}

void onClose() {
	cube->close();
	sphere->close();
}
	
}

};