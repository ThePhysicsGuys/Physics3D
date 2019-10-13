#pragma once

#include "profilerUI.h"

#include "../physics/datastructures/buffers.h"

#include "../../debug.h"

class PointMesh;
class VectorMesh;
class Font;

enum GraphicsProcess {
	UPDATE,
	SKYBOX,
	VECTORS,
	LIGHTING,
	WAIT_FOR_LOCK,
	PHYSICALS,
	ORIGIN,
	PICKER,
	PROFILER,
	FINALIZE,
	OTHER,
	COUNT
};

extern const char* const graphicsDebugLabels[];
extern BreakdownAverageProfiler<60, GraphicsProcess> graphicsMeasure;

namespace Debug {

	enum class SphereColissionRenderMode : int {
		NONE,
		SELECTED,
		ALL
	};

	enum class ColTreeRenderMode : int {
		NONE,
		SELECTED,
		FREE,
		TERRAIN,
		ALL
	};

	extern bool renderPiesEnabled;
	extern AddableBuffer<float> visibleVectors; 
	extern std::map<VectorType, bool> vectorDebugEnabled;
	extern std::map<PointType, bool> pointDebugEnabled;

	extern SphereColissionRenderMode colissionSpheresMode;
	extern ColTreeRenderMode colTreeRenderMode;
	extern int fieldIndex;

	void toggleVectorType(VectorType type);
	void togglePointType(PointType type);
	void updateVectorMesh(VectorMesh* vectorMesh, AppDebug::ColoredVector* data, size_t size);
	void updatePointMesh(PointMesh* pointMesh, AppDebug::ColoredPoint* data, size_t size);

	void addDebugField(Vec2 dimension, Font* font, const char* varName, std::string value, const char* unit);
	size_t getTheoreticalNumberOfIntersections(size_t objCount);
	std::string toString(std::chrono::nanoseconds t);
	void renderDebugFields();

	template<typename T>
	void addDebugField(Vec2 dimension, Font* font, const char* varName, T value, const char* unit) {
		addDebugField(dimension, font, varName, std::to_string(value), unit);
	}

	template<size_t N, typename EnumType>
	PieChart toPieChart(BreakdownAverageProfiler<N, EnumType>& profiler, const char* title, Vec2f piePosition, float pieSize) {
		auto results = profiler.history.avg();
		auto averageTotalTime = results.sum();

		PieChart chart(title, toString(averageTotalTime), piePosition, pieSize);

		for (size_t i = 0; i < profiler.size(); i++) {
			DataPoint p = DataPoint(static_cast<float>(results[i].count()), toString(results[i]), pieColors[i], profiler.labels[i]);
			chart.add(p);
		}

		return chart;
	}

	template<size_t N, typename Unit, typename EnumType>
	PieChart toPieChart(HistoricTally<N, Unit, EnumType>& tally, const char* title, Vec2f piePosition, float pieSize) {
		int sum = 0;
		for (auto entry : tally.history) 
			sum += (int) entry.sum();
		
		auto results = tally.history.avg();
		Unit avgTotal = (tally.history.size() != 0) ? (sum / tally.history.size()) : 0;

		PieChart chart(title, std::to_string(avgTotal), piePosition, pieSize);

		for (size_t i = 0; i < tally.size(); i++) {
			DataPoint p = DataPoint(static_cast<float>(results[i]), std::to_string(results[i]), pieColors[i], tally.labels[i]);
			chart.add(p);
		}

		return chart;
	}
}