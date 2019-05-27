#pragma once

#include "visualDebug.h"
#include "gui\gui.h"
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

std::map<VecType, bool> debug_enabled {
	{ INFO, false }, 
	{ VELOCITY, false }, 
	{ ACCELERATION, false }, 
	{ FORCE, false }, 
	{ ANGULAR_IMPULSE, false }, 
	{ POSITION, false }, 
	{ MOMENT, false }, 
	{ IMPULSE, false }, 
	{ ANGULAR_VELOCITY , false }
};

std::map<VecType, Vec3f> vecColors {
	{ INFO, Vec3f(0,1,0) },
	{ VELOCITY, Vec3f(0,0,1) },
	{ ACCELERATION, Vec3f(0,1,1) },
	{ FORCE, Vec3f(1, 0, 0) },
	{ POSITION, Vec3f(1,1,0) },
	{ MOMENT, Vec3f(1,0,1) },
	{ IMPULSE, Vec3f(0.5,0.7,1) },
	{ ANGULAR_VELOCITY , Vec3f(0.75) },
	{ ANGULAR_IMPULSE, Vec3f(0.8,0.1,0.4) }
};

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

AddableBuffer<float> visibleVecs(900);

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
			visibleVecs.add(vecColors[v.type].x);
			visibleVecs.add(vecColors[v.type].y);
			visibleVecs.add(vecColors[v.type].z);
		}
	}

	vectorMesh->update(visibleVecs.data, visibleVecs.index / 9);
}