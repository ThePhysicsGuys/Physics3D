#pragma once

#include "visualDebug.h"
#include "../debug.h"
#include "../buffers.h"
#include "profilerUI.h"
#include "font.h"

const char* const graphicsDebugLabels[]{
	"Update",
	"Skybox",
	"Vectors",
	"Physicals",
	"Lighting",
	"Origin",
	"Profiler",
	"Finalize",
	"Other"
};

BreakdownAverageProfiler<60, GraphicsProcess> graphicsMeasure = BreakdownAverageProfiler<60, GraphicsProcess>(graphicsDebugLabels);

// Debug
using namespace Debug;
std::map<VecType, bool> debug_enabled{{INFO, false}, {VELOCITY, false}, {ACCELERATION, false}, {FORCE, false}, {ANGULAR_IMPULSE, false}, {POSITION, false}, {MOMENT, false}, {IMPULSE, false}, {ANGULAR_VELOCITY , false}};
std::map<VecType, double> vecColors{ {INFO, 0.15}, {VELOCITY, 0.3},{ACCELERATION, 0.35},{FORCE, 0.0}, {POSITION, 0.5}, {MOMENT, 0.1}, {IMPULSE, 0.7}, {ANGULAR_VELOCITY , 0.75} ,{ANGULAR_IMPULSE, 0.8}};
bool renderPies = false;

int fieldIndex = 0;

void toggleDebugVecType(Debug::VecType t) {
	debug_enabled[t] = !debug_enabled[t];
}

std::string toString(std::chrono::nanoseconds t) {
	std::stringstream ss;
	ss.precision(4);
	ss << t.count() * 0.000001f;
	ss << "ms";
	return ss.str();
}

size_t getTheoreticalNumberOfIntersections(size_t objCount) {
	return (objCount-1)*objCount / 2;
}

AddableBuffer<float> visibleVecs(700);

void updateVecMesh(VectorMesh* vectorMesh, AppDebug::ColoredVec* data, size_t size) {
	visibleVecs.clear();

	for (size_t i = 0; i < size; i++) {
		const AppDebug::ColoredVec& v = data[i];
		if (debug_enabled[v.type]) {
			visibleVecs.add(v.origin.x);
			visibleVecs.add(v.origin.y);
			visibleVecs.add(v.origin.z);
			visibleVecs.add(v.vec.x);
			visibleVecs.add(v.vec.y);
			visibleVecs.add(v.vec.z);
			visibleVecs.add(vecColors[v.type]);
		}
	}

	vectorMesh->update(visibleVecs.data, visibleVecs.index / 7);
}