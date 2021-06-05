#include "worldBenchmark.h"

#include <Physics3D/world.h>
#include <Physics3D/geometry/shapeLibrary.h>
#include <Physics3D/math/linalg/commonMatrices.h>

#include <Physics3D/geometry/shapeCreation.h>

namespace P3D {
class BasicWorldBenchmark : public WorldBenchmark {
public:
	BasicWorldBenchmark() : WorldBenchmark("basicWorld", 1000) {}

	void init() {
		createFloor(50, 50, 10);
		//Polyhedron cube = ShapeLibrary::createCube(0.9);

		for(int x = -5; x < 5; x++) {
			for(int y = 0; y < 5; y++) {
				for(int z = -5; z < 5; z++) {
					world.addPart(new Part(boxShape(0.9, 0.9, 0.9), GlobalCFrame(x, y + 1.0, z), {1.0, 0.7, 0.5}));
				}
			}
		}
	}
} basicWorld;
};

