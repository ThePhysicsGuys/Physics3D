
#include <Physics3D/geometry/shapeLibrary.h>
#include <Physics3D/geometry/shape.h>
#include <Physics3D/geometry/shapeCreation.h>
#include "worldBenchmark.h"
#include <Physics3D/math/linalg/commonMatrices.h>
#include <Physics3D/math/linalg/trigonometry.h>

namespace P3D {
class ManyCubesBenchmark : public WorldBenchmark {
public:
	ManyCubesBenchmark() : WorldBenchmark("manyCubes", 10000) {}

	void init() {
		createFloor(50, 50, 10);

		int minX = -5;
		int maxX = 5;
		int minY = -5;
		int maxY = 5;
		int minZ = -5;
		int maxZ = 5;

		GlobalCFrame ref(0, 15, 0, Rotation::fromEulerAngles(3.1415 / 4, 3.1415 / 4, 0.0));

		//Part* newCube = new Part(boxShape(1.0, 1.0, 1.0), ref.localToGlobal(CFrame(0,0,0)), {1.0, 0.2, 0.5});
		//world.addPart(newCube);

		for(double x = minX; x < maxX; x += 1.01) {
			for(double y = minY; y < maxY; y += 1.01) {
				for(double z = minZ; z < maxZ; z += 1.01) {
					Part* newCube = new Part(polyhedronShape(ShapeLibrary::createBox(1.0, 1.0, 1.0)), ref.localToGlobal(CFrame(x, y, z)), {1.0, 0.2, 0.5});
					world.addPart(newCube);
				}
			}
		}
	}
} manyCubesBench;
};
