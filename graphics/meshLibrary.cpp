#include "core.h"

#include "meshLibrary.h"

#include <sstream>

#include "mesh/indexedMesh.h"

#include "../physics/misc/shapeLibrary.h"
#include "../engine/io/import.h"
#include "visualShape.h"

namespace Graphics {

namespace Library {
IndexedMesh* cube = nullptr;
IndexedMesh* sphere = nullptr;

void onInit() {
	// Cube
	cube = new IndexedMesh(VisualShape(::Library::createCube(1)));

	// Sphere
	sphere = new IndexedMesh(VisualShape(::Library::createSphere(1,5)));
}

void onClose() {
	cube->close();
	sphere->close();
}
}

};