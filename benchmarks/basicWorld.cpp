#include "worldBenchmark.h"

#include "../physics/world.h"
#include "../physics/misc/shapeLibrary.h"
#include "../physics/math/linalg/commonMatrices.h"

#include "../physics/geometry/basicShapes.h"

class BasicWorldBenchmark : public WorldBenchmark {
public:
	BasicWorldBenchmark() : WorldBenchmark("basicWorld", 10000) {}

	void init() {
		createFloor(50, 50, 10);
		//Polyhedron cube = Library::createCube(0.9);

		for (int x = -5; x < 5; x++) {
			for (int y = 0; y < 5; y++) {
				for (int z = -5; z < 5; z++) {
					world.addPart(new Part(Box(0.9, 0.9, 0.9), GlobalCFrame(x, y + 1.0, z), {1.0, 0.7, 0.5}));
				}
			}
		}
	}
} basicWorld;

