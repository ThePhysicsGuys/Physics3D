#include "worldBenchmark.h"

#include <Physics3D/world.h>
#include <Physics3D/geometry/shapeLibrary.h>
#include <Physics3D/geometry/shapeCreation.h>
#include <Physics3D/math/linalg/commonMatrices.h>

namespace P3D {
class ComplexObjectBenchmark : public WorldBenchmark {
public:
	ComplexObjectBenchmark() : WorldBenchmark("complexObject", 10000) {}

	void init() {
		createFloor(50, 50, 10);
		Polyhedron object = ShapeLibrary::icosahedron;
		world.addPart(new Part(polyhedronShape(ShapeLibrary::createSphere(1.0, 7)), GlobalCFrame(0, 2.0, 0), basicProperties));
	}
} complexObjectBench;
};
