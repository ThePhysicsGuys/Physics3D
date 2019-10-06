#include "worldBenchmark.h"

#include "../engine/world.h"
#include "../engine/misc/shapeLibrary.h"
#include "../engine/math/linalg/commonMatrices.h"

class BasicWorldBenchmark : public WorldBenchmark {
public:
	BasicWorldBenchmark() : WorldBenchmark(10000) {
		Polyhedron cube = Library::createCube(0.9);
		Polyhedron floor = Library::createBox(50.0, 0.5, 50.0);
		Polyhedron wall = Library::createBox(0.2, 10.0, 50.0);

		PartProperties basicProperties{ 1.0, 0.7, 0.5 };

		world.addTerrainPart(new Part(floor, GlobalCFrame(0.0, 0.0, 0.0), basicProperties));
		world.addTerrainPart(new Part(wall, GlobalCFrame(50.0, 5.0, 0.0), basicProperties));
		world.addTerrainPart(new Part(wall, GlobalCFrame(-50.0, 5.0, 0.0), basicProperties));
		world.addTerrainPart(new Part(wall, GlobalCFrame(0.0, 5.0, 50.0, ROT_Y_90(double)), basicProperties));
		world.addTerrainPart(new Part(wall, GlobalCFrame(0.0, 5.0, -50.0, ROT_Y_90(double)), basicProperties));

		for (int x = -5; x < 5; x++) {
			for (int y = 0; y < 5; y++) {
				for (int z = -5; z < 5; z++) {
					world.addPart(new Part(cube, GlobalCFrame(x, y + 1.0, z), basicProperties));
				}
			}
		}
	}
};

Benchmark* basicWorld() { return new BasicWorldBenchmark(); }
