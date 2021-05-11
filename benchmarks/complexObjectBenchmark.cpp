#include "worldBenchmark.h"

#include "../physics/world.h"
#include "../physics/misc/shapeLibrary.h"
#include "../physics/geometry/shapeCreation.h"
#include "../physics/math/linalg/commonMatrices.h"

namespace P3D {
class ComplexObjectBenchmark : public WorldBenchmark {
public:
	ComplexObjectBenchmark() : WorldBenchmark("complexObject", 10000) {}

	void init() {
		createFloor(50, 50, 10);
		Polyhedron object = Library::icosahedron;
		world.addPart(new Part(polyhedronShape(Library::createSphere(1.0, 7)), GlobalCFrame(0, 2.0, 0), basicProperties));
	}
} complexObjectBench;
};
