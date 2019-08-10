#include "meshLibrary.h"

#include <sstream>

#include "view/mesh/indexedMesh.h"

#include "resourceManager.h"
#include "shapeLibrary.h"
#include "visualShape.h"

#include "io/import.h"

namespace Library {
	IndexedMesh* cube = nullptr;
	IndexedMesh* sphere = nullptr;

	void init() {
		// Cube
		cube = new IndexedMesh(VisualShape(createCube(1)));
		
		// Sphere
		VisualShape sphereShape(OBJImport::load((std::istream&) std::istringstream(getResourceAsString(SPHERE_MODEL))));
		sphere = new IndexedMesh(sphereShape);
	}

	void close() {
		cube->close();
		sphere->close();
	}
}