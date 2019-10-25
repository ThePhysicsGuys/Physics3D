#include "benchmark.h"

#include "../physics/geometry/polyhedron.h"
#include "../physics/misc/shapeLibrary.h"
#include "../physics/math/linalg/trigonometry.h"

class GetBounds : public Benchmark {
	Polyhedron poly;
public:
	GetBounds() : Benchmark("getBounds") {}

	void init() override { this->poly = Library::createBox(1.0, 20.0, 300.0); }
	void run() override {
		Mat3 m = fromEulerAngles(0.1, 0.05, 0.7);
		for(size_t i = 0; i < 1000000000; i++) {
			//Mat3 m = fromEulerAngles(0.1 * (i%5), 0.2 * (i%6), 0.3 * (i%7));

			this->poly.getBounds(m);
		}
	}
} getBounds;



