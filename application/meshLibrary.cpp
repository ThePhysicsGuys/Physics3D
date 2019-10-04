#include "core.h"

#include "meshLibrary.h"

#include <sstream>

#include "view/mesh/indexedMesh.h"

#include "resourceLoader.h"
#include "shapeLibrary.h"
#include "visualShape.h"

#include "io/import.h"

namespace Library {
	IndexedMesh* cube = nullptr;
	IndexedMesh* sphere = nullptr;
	IndexedMesh* vector = nullptr;

	void onInit() {
		// Cube
		cube = new IndexedMesh(VisualShape(createCube(1)));
		
		// Sphere
		VisualShape sphereShape(OBJImport::load((std::istream&) std::istringstream(getResourceAsString(SPHERE_MODEL))));
		sphere = new IndexedMesh(sphereShape);

		VisualShape vectorShape = OBJImport::load("../res/models/gui/translate_shaft.obj");
		vector = new IndexedMesh(vectorShape);
	}

	void onClose() {
		cube->close();
		sphere->close();
		vector->close();
	}
}