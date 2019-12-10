#include "worldBenchmark.h"

#include "../util/log.h"
#include "../physics/physicsProfiler.h"
#include <iostream>
#include <sstream>
#include "../physics/misc/gravityForce.h"

#include "../physics/geometry/basicShapes.h"
#include "../physics/math/linalg/commonMatrices.h"

#include "../physics/misc/shapeLibrary.h"

#include "../physics/misc/filters/outOfBoundsFilter.h"

WorldBenchmark::WorldBenchmark(const char* name, int tickCount) : Benchmark(name), world(0.005), tickCount(tickCount) {
	world.addExternalForce(new ExternalGravity(Vec3(0, -10, 0)));
}

void WorldBenchmark::run() {
	world.isValid();
	Part& partToTrack = (*world.physicals[0])[0];
	for (int i = 0; i < tickCount; i++) {
		if (i % (tickCount / 8) == 0) {
			Log::print("Tick %d\n", i);
			Position pos = partToTrack.getCFrame().getPosition();
			Log::print("Location of object: %.5f %.5f %.5f\n", double(pos.x), double(pos.y), double(pos.z));

			size_t partsOutOfBounds = 0;
			for(const Part& p : world.iterPartsFiltered(OutOfBoundsFilter(Bounds(Position(-100.0, -100.0, -100.0), Position(100.0, 100.0, 100.0))))) {
				partsOutOfBounds++;
			}

			Log::print("%d/%d parts out of bounds!\n", partsOutOfBounds, world.getPartCount());
		}

		physicsMeasure.mark(PhysicsProcess::OTHER);

		world.tick();

		physicsMeasure.end();

		GJKCollidesIterationStatistics.nextTally();
		GJKNoCollidesIterationStatistics.nextTally();
		EPAIterationStatistics.nextTally();
	}
	world.isValid();
}

static const size_t LABEL_LENGTH = 23;
static const size_t COUNT_LENGTH = 11;
static const size_t FRACTION_LENGTH = 6;
static const size_t BAR_LENGTH = 36;

void printToLength(std::string text, size_t length) {
	std::cout << text;
	Log::setColor(Log::BLACK);
	for (size_t i = text.size(); i < length; i++) {
		std::cout << ' ';
	}
}

static int colors[]{
	Log::STRONG | Log::BLUE,
	Log::STRONG | Log::RED,
	Log::STRONG | Log::YELLOW,
	Log::STRONG | Log::MAGENTA,
	Log::STRONG | Log::GREEN,
	Log::STRONG | Log::AQUA,
	Log::STRONG | Log::WHITE,
	/*Log::RED,
	Log::GREEN,
	Log::BLUE,
	Log::YELLOW,
	Log::MAGENTA,
	Log::AQUA,
	Log::WHITE,*/
};

int getColor(size_t i) {
	//return (colors[i] >= 0x7) ? colors[i] : colors[i] | (Log::WHITE << 4);
	return colors[i % 7];
}

int getBGColor(size_t i) {
	return colors[i % 7] << 4 | colors[i % 7];
}

template<typename T>
void printBreakdown(const T* values, const char** labels, size_t N, std::string unit) {
	
	T total = values[0];
	T max = values[0];

	for (size_t i = 1; i < N; i++) {
		total += values[i];
		max = (values[i] > max) ? values[i] : max;
	}

	for (size_t i = 0; i < N; i++) {
		T v = values[i];
		double fractionOfTotal = double(v) / total;
		double fractionOfMax = double(v) / max;

		Log::setColor(getColor(i));

		printToLength(labels[i] + std::string(":"), LABEL_LENGTH);

		std::stringstream ss;
		ss.precision(5);
		ss << std::fixed;
		ss << v;
		ss << unit;

		Log::setColor(getColor(i));
		printToLength(ss.str(), COUNT_LENGTH);

		std::stringstream ss2;
		ss2.precision(2);
		ss2 << std::fixed;
		ss2 << (fractionOfTotal * 100);
		ss2 << "%";

		Log::setColor(getColor(i));
		printToLength(ss2.str(), FRACTION_LENGTH);

		Log::setColor(Log::BLACK);
		std::cout << ' ';
		Log::setColor(Log::WHITE << 4);
		std::cout << ' ';

		size_t thisBarLength = std::ceil(BAR_LENGTH * fractionOfMax);

		Log::setColor(getBGColor(i));
		for (size_t i = 0; i < thisBarLength; i++) {
			std::cout << '=';
		}
		Log::setColor(Log::BLACK);
		std::cout << '|';

		std::cout << '\n';
	}

}

void WorldBenchmark::printResults(double timeTakenMillis) {
	double tickTime = (timeTakenMillis) / tickCount;
	Log::print("%d ticks at %f ticks per second\n", tickCount, 1000 / tickTime);

	auto physicsBreakdown = physicsMeasure.history.avg();

	double millis[physicsMeasure.size()];

	for (size_t i = 0; i < physicsMeasure.size(); i++) {
		millis[i] = physicsBreakdown[i].count() / 1000000.0;
	}

	Log::setColor(Log::WHITE);
	std::cout << "\n";
	Log::setColor(Log::STRONG | Log::MAGENTA);
	std::cout << "[Physics Profiler]\n";
	printBreakdown(millis, physicsMeasure.labels, physicsMeasure.size(), "ms");

	Log::setColor(Log::WHITE);
	std::cout << "\n";
	Log::setColor(Log::STRONG | Log::MAGENTA);
	std::cout << "[Intersection Statistics]\n";
	printBreakdown(intersectionStatistics.history.avg().values, intersectionStatistics.labels, intersectionStatistics.size(), "");
}


void WorldBenchmark::createFloor(double w, double h, double wallHeight) {
	world.addTerrainPart(new Part(Library::createBox(w, 1.0, h), GlobalCFrame(0.0, 0.0, 0.0), basicProperties));
	world.addTerrainPart(new Part(Library::createBox(0.8, wallHeight, h), GlobalCFrame(w, wallHeight/2, 0.0), basicProperties));
	world.addTerrainPart(new Part(Library::createBox(0.8, wallHeight, h), GlobalCFrame(-w, wallHeight / 2, 0.0), basicProperties));
	world.addTerrainPart(new Part(Library::createBox(w, wallHeight, 0.8), GlobalCFrame(0.0, wallHeight / 2, h), basicProperties));
	world.addTerrainPart(new Part(Library::createBox(w, wallHeight, 0.8), GlobalCFrame(0.0, wallHeight / 2, -h), basicProperties));
}
