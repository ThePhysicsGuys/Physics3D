#pragma once

#include "visualDebug.h"
#include "profilerUI.h"

#include "../font.h"
#include "../gui/gui.h"

#include "../../debug.h"
#include "../../threePhaseBuffer.h"

const char* const graphicsDebugLabels[]{
	"Update",
	"Skybox",
	"Vectors",
	"Lighting",
	"Physicals",
	"Origin",
	"Profiler",
	"Finalize",
	"Other"
};

BreakdownAverageProfiler<60, GraphicsProcess> graphicsMeasure = BreakdownAverageProfiler<60, GraphicsProcess>(graphicsDebugLabels);

// Debug
using namespace Debug;

std::map<VectorType, bool> debug_enabled {
	{ INFO_VEC, false }, 
	{ VELOCITY, false }, 
	{ ACCELERATION, false }, 
	{ FORCE, false }, 
	{ ANGULAR_IMPULSE, false }, 
	{ POSITION, false }, 
	{ MOMENT, false }, 
	{ IMPULSE, false }, 
	{ ANGULAR_VELOCITY , false }
};

std::map<PointType, bool> point_debug_enabled{
	{INFO_POINT, false},
	{CENTER_OF_MASS, false},
	{INTERSECTION, false},
};

std::map<VectorType, Vec3f> vecColors {
	{ INFO_VEC, Vec3f(0,1,0) },
	{ VELOCITY, Vec3f(0,0,1) },
	{ ACCELERATION, Vec3f(0,1,1) },
	{ FORCE, Vec3f(1, 0, 0) },
	{ POSITION, Vec3f(1,1,0) },
	{ MOMENT, Vec3f(1,0,1) },
	{ IMPULSE, Vec3f(0.5,0.7,1) },
	{ ANGULAR_VELOCITY , Vec3f(0.75) },
	{ ANGULAR_IMPULSE, Vec3f(0.8,0.1,0.4) }
};

struct PointColorPair {
	Vec3f color1;
	Vec3f color2;
};

std::map<PointType, PointColorPair> pointColors {
	{ INFO_POINT, PointColorPair { Vec3f(1.0f, 0.5f, 0.0f), Vec3f(1.0f, 0.2f, 0.0f) }},
	{ CENTER_OF_MASS, PointColorPair { Vec3f(1.0f, 1.0f, 0.0f), Vec3f(0.0f, 0.0f, 0.0f) }},
	{ INTERSECTION, PointColorPair { Vec3f(0.0f, 0.0f, 1.0f), Vec3f(0.0f, 1.0f, 0.0f) }},
};

bool renderPiesEnabled = false;
SphereColissionRenderMode colissionSpheresMode;
ColTreeRenderMode renderColTree;

int fieldIndex = 0;

void toggleDebugVecType(Debug::VectorType t) {
	debug_enabled[t] = !debug_enabled[t];
}
void toggleDebugPointType(Debug::PointType t) {
	point_debug_enabled[t] = !point_debug_enabled[t];
}

std::string toString(std::chrono::nanoseconds t) {
	std::stringstream ss;
	ss.precision(4);
	ss << t.count() * 0.000001f;
	ss << "ms";
	return ss.str();
}

size_t getTheoreticalNumberOfIntersections(size_t objCount) {
	return (objCount - 1) * objCount / 2;
}

AddableBuffer<float> visibleVecs(900);
AddableBuffer<float> visiblePoints(1000);

void updateVecMesh(VectorMesh* vectorMesh, AppDebug::ColoredVector* data, size_t size) {
	visibleVecs.clear();

	for (size_t i = 0; i < size; i++) {
		const AppDebug::ColoredVector& v = data[i];
		if (debug_enabled[v.type]) {
			visibleVecs.add(v.origin.x);
			visibleVecs.add(v.origin.y);
			visibleVecs.add(v.origin.z);
			visibleVecs.add(v.vector.x);
			visibleVecs.add(v.vector.y);
			visibleVecs.add(v.vector.z);
			visibleVecs.add(vecColors[v.type].x);
			visibleVecs.add(vecColors[v.type].y);
			visibleVecs.add(vecColors[v.type].z);
		}
	}

	vectorMesh->update(visibleVecs.data, visibleVecs.size / 9);
}

void updatePointMesh(PointMesh* pointMesh, AppDebug::ColoredPoint* data, size_t size) {
	visiblePoints.clear();

	for (size_t i = 0; i < size; i++) {
		const AppDebug::ColoredPoint& p = data[i];
		if (point_debug_enabled[p.type]) {
			Vec3f color1 = pointColors[p.type].color1;
			Vec3f color2 = pointColors[p.type].color2;

			visiblePoints.add(p.point.x);
			visiblePoints.add(p.point.y);
			visiblePoints.add(p.point.z);
			visiblePoints.add(0.025);
			visiblePoints.add(color1.x);
			visiblePoints.add(color1.y);
			visiblePoints.add(color1.z);
			visiblePoints.add(color2.x);
			visiblePoints.add(color2.y);
			visiblePoints.add(color2.z);
		}
	}

	pointMesh->update(visiblePoints.data, visiblePoints.size / 10);
}