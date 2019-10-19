#include "worldBenchmark.h"

#include "../physics/world.h"
#include "../physics/misc/shapeLibrary.h"
#include "../physics/math/linalg/commonMatrices.h"

class ComplexObjectBenchmark : public WorldBenchmark {
public:
	ComplexObjectBenchmark() : WorldBenchmark(10000) {
		Polyhedron object = Library::icosahedron;
		Polyhedron floor = Library::createBox(50.0, 0.5, 50.0);
		Polyhedron wall = Library::createBox(0.2, 10.0, 50.0);

		PartProperties basicProperties{1.0, 0.7, 0.5};

		world.addTerrainPart(new Part(floor, GlobalCFrame(0.0, 0.0, 0.0), basicProperties));
		world.addTerrainPart(new Part(wall, GlobalCFrame(50.0, 5.0, 0.0), basicProperties));
		world.addTerrainPart(new Part(wall, GlobalCFrame(-50.0, 5.0, 0.0), basicProperties));
		world.addTerrainPart(new Part(wall, GlobalCFrame(0.0, 5.0, 50.0, ROT_Y_90(double)), basicProperties));
		world.addTerrainPart(new Part(wall, GlobalCFrame(0.0, 5.0, -50.0, ROT_Y_90(double)), basicProperties));

		world.addPart(new Part(Library::createSphere(1.0, 7), GlobalCFrame(0, 2.0, 0), {1.0, 1.0, 1.0}));
	}
};

Benchmark* complexObject() { return new ComplexObjectBenchmark(); }
