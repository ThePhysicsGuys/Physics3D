#pragma once

#include "profilerUI.h"

#include "../engine/math/mat3.h"
#include "../engine/datastructures/buffers.h"

#include "../../debug.h"

class PointMesh;
class VectorMesh;
class Font;

extern const char* const graphicsDebugLabels[];

enum GraphicsProcess {
	UPDATE,
	SKYBOX,
	VECTORS,
	LIGHTING,
	WAIT_FOR_LOCK,
	PHYSICALS,
	ORIGIN,
	PROFILER,
	FINALIZE,
	OTHER,
	COUNT
};

enum class SphereColissionRenderMode : int {
	NONE,
	SELECTED,
	ALL
};

enum class ColTreeRenderMode : int {
	NONE,
	SELECTED,
	ALL
};

extern BreakdownAverageProfiler<60, GraphicsProcess> graphicsMeasure;

// Debug
using namespace Debug;
extern std::map<VectorType, bool> debug_enabled;
extern std::map<PointType, bool> point_debug_enabled;

extern bool renderPiesEnabled;
extern SphereColissionRenderMode colissionSpheresMode;
extern int fieldIndex;
extern ColTreeRenderMode renderColTree;

void toggleDebugVecType(Debug::VectorType t);
void toggleDebugPointType(Debug::PointType t);

void addDebugField(Vec2 dimension, Font* font, const char* varName, std::string value, const char* unit);

template<typename T>
void addDebugField(Vec2 dimension, Font* font, const char* varName, T value, const char* unit) {
	addDebugField(dimension, font, varName, std::to_string(value), unit);
}

void renderDebugFields();

std::string toString(std::chrono::nanoseconds t);

template<size_t N, typename EnumType>
PieChart toPieChart(BreakdownAverageProfiler<N, EnumType>& profiler, const char* title, Vec2f piePosition, float pieSize) {
	auto results = profiler.history.avg();
	std::chrono::nanoseconds avgTotalTime = results.sum();

	PieChart chart(title, toString(avgTotalTime), piePosition, pieSize);
	for (size_t i = 0; i < profiler.size(); i++) {
		float weight = static_cast<float>(results[i].count());
		DataPoint p = DataPoint(weight, toString(results[i]), pieColors[i], profiler.labels[i]);
		chart.add(p);
	}

	return chart;
}

template<size_t N, typename Unit, typename EnumType>
PieChart toPieChart(HistoricTally<N, Unit, EnumType>& tally, const char* title, Vec2f piePosition, float pieSize) {
	auto sum = 0;
	for (auto entry : tally.history) {
		sum += int(entry.sum());
	}
	auto results = tally.history.avg();
	Unit avgTotal = (tally.history.size() != 0) ? (sum / tally.history.size()) : 0;
	//Unit avgTotal = results.sum();

	PieChart chart(title, std::to_string(avgTotal), piePosition, pieSize);
	for (size_t i = 0; i < tally.size(); i++) {
		float result = results[i] * 1.0f;
		DataPoint p = DataPoint(result, std::to_string(results[i]), pieColors[i], tally.labels[i]);
		chart.add(p);
	}

	return chart;
}

size_t getTheoreticalNumberOfIntersections(size_t objCount);

extern AddableBuffer<float> visibleVecs;

void updateVecMesh(VectorMesh* vectorMesh, AppDebug::ColoredVector* data, size_t size);
void updatePointMesh(PointMesh* pointMesh, AppDebug::ColoredPoint* data, size_t size);