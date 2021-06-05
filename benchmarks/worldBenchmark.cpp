#include "worldBenchmark.h"

#include "../util/log.h"
#include "../util/terminalColor.h"
#include <iostream>
#include <sstream>
#include <cstddef>
#include <Physics3D/externalforces/gravityForce.h>

#include <Physics3D/geometry/shape.h>
#include <Physics3D/geometry/shapeCreation.h>
#include <Physics3D/math/linalg/commonMatrices.h>

#include <Physics3D/geometry/shapeLibrary.h>
#include <Physics3D/boundstree/filters/outOfBoundsFilter.h>
#include <Physics3D/misc/physicsProfiler.h>

namespace P3D {
WorldBenchmark::WorldBenchmark(const char* name, int tickCount) : Benchmark(name), world(0.005), tickCount(tickCount) {
	world.addExternalForce(new DirectionalGravity(Vec3(0, -10, 0)));
}

void WorldBenchmark::run() {
	world.isValid();
	Part& partToTrack = *world.physicals[0]->getMainPart();
	for(int i = 0; i < tickCount; i++) {
		if(i % (tickCount / 8) == 0) {
			Log::print("Tick %d\n", i);
			Position pos = partToTrack.getCFrame().getPosition();
			Log::print("Location of object: %.5f %.5f %.5f\n", double(pos.x), double(pos.y), double(pos.z));

			std::size_t partsOutOfBounds = 0;
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

static const std::size_t LABEL_LENGTH = 23;
static const std::size_t COUNT_LENGTH = 11;
static const std::size_t FRACTION_LENGTH = 6;
static const std::size_t BAR_LENGTH = 36;

void printToLength(std::string text, std::size_t length) {
	std::cout << text;
	setColor(TerminalColor::BLACK);
	for(std::size_t i = text.size(); i < length; i++) {
		std::cout << ' ';
	}
}

static TerminalColor colors[]{
	TerminalColor::BLUE,
	TerminalColor::RED,
	TerminalColor::YELLOW,
	TerminalColor::MAGENTA,
	TerminalColor::GREEN,
	TerminalColor::CYAN,
	TerminalColor::WHITE,
	/*Log::RED,
	Log::GREEN,
	Log::BLUE,
	Log::YELLOW,
	Log::MAGENTA,
	Log::AQUA,
	Log::WHITE,*/
};

TerminalColor getColor(std::size_t i) {
	//return (colors[i] >= 0x7) ? colors[i] : colors[i] | (Log::WHITE << 4);
	return colors[i % 7];
}

/*int getBGColor(std::size_t i) {
	return colors[i % 7] << 4 | colors[i % 7];
}*/

template<typename T>
void printBreakdown(const T* values, const char** labels, std::size_t N, std::string unit) {

	T total = values[0];
	T max = values[0];

	for(std::size_t i = 1; i < N; i++) {
		total += values[i];
		max = (values[i] > max) ? values[i] : max;
	}

	for(std::size_t i = 0; i < N; i++) {
		T v = values[i];
		double fractionOfTotal = total != 0 ? double(v) / total : 0.0;
		double fractionOfMax = total != 0 ? double(v) / max : 0.0;

		setColor(getColor(i));

		printToLength(labels[i] + std::string(":"), LABEL_LENGTH);

		std::stringstream ss;
		ss.precision(5);
		ss << std::fixed;
		ss << v;
		ss << unit;

		setColor(getColor(i));
		printToLength(ss.str(), COUNT_LENGTH);

		std::stringstream ss2;
		ss2.precision(2);
		ss2 << std::fixed;
		ss2 << (fractionOfTotal * 100);
		ss2 << "%";

		setColor(getColor(i));
		printToLength(ss2.str(), FRACTION_LENGTH);

		setColor(TerminalColor::BLACK);
		std::cout << ' ';
		setColor(TerminalColor::WHITE, TerminalColor::WHITE);
		std::cout << ' ';

		std::size_t thisBarLength = static_cast<std::size_t>(std::ceil(BAR_LENGTH * fractionOfMax));

		setColor(getColor(i), getColor(i));
		for(std::size_t i = 0; i < thisBarLength; i++) {
			std::cout << '=';
		}
		setColor(TerminalColor::BLACK);
		std::cout << '|';

		std::cout << '\n';
	}

}

void WorldBenchmark::printResults(double timeTakenMillis) {
	double tickTime = (timeTakenMillis) / tickCount;
	Log::print("%d ticks at %f ticks per second\n", tickCount, 1000 / tickTime);

	auto physicsBreakdown = physicsMeasure.history.avg();

	double millis[physicsMeasure.size()];

	for(std::size_t i = 0; i < physicsMeasure.size(); i++) {
		millis[i] = physicsBreakdown[i].count() / 1000000.0;
	}

	setColor(TerminalColor::WHITE);
	std::cout << "\n";
	setColor(TerminalColor::MAGENTA);
	std::cout << "[Physics Profiler]\n";
	printBreakdown(millis, physicsMeasure.labels, physicsMeasure.size(), "ms");

	setColor(TerminalColor::WHITE);
	std::cout << "\n";
	setColor(TerminalColor::MAGENTA);
	std::cout << "[Intersection Statistics]\n";
	printBreakdown(intersectionStatistics.history.avg().values, intersectionStatistics.labels, intersectionStatistics.size(), "");
	setColor(TerminalColor::WHITE);
}


void WorldBenchmark::createFloor(double w, double h, double wallHeight) {
	world.addTerrainPart(new Part(boxShape(w, 1.0, h), GlobalCFrame(0.0, 0.0, 0.0), basicProperties));
	world.addTerrainPart(new Part(boxShape(0.8, wallHeight, h), GlobalCFrame(w, wallHeight / 2, 0.0), basicProperties));
	world.addTerrainPart(new Part(boxShape(0.8, wallHeight, h), GlobalCFrame(-w, wallHeight / 2, 0.0), basicProperties));
	world.addTerrainPart(new Part(boxShape(w, wallHeight, 0.8), GlobalCFrame(0.0, wallHeight / 2, h), basicProperties));
	world.addTerrainPart(new Part(boxShape(w, wallHeight, 0.8), GlobalCFrame(0.0, wallHeight / 2, -h), basicProperties));
}
};
