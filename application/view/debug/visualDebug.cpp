#include "core.h"

#include "visualDebug.h"

#include "../mesh/vectorMesh.h"
#include "../mesh/pointMesh.h"

#include "../font.h"
#include "../batch/commandBatch.h"
#include "../path/path.h"
#include "../gui/gui.h"

#include "../../threePhaseBuffer.h"
#include "../batch/commandBatch.h"

#include <sstream>

const char* const graphicsDebugLabels[]{
		"Update",
		"Skybox",
		"Vectors",
		"Lighting",
		"Wait For Lock",
		"Physicals",
		"Origin",
		"Profiler",
		"Finalize",
		"Other"
};

BreakdownAverageProfiler<60, GraphicsProcess> graphicsMeasure = BreakdownAverageProfiler<60, GraphicsProcess>(graphicsDebugLabels);

namespace Debug {

	std::map<VectorType, bool> vectorDebugEnabled {
		{ INFO_VEC        , false },
		{ VELOCITY        , false },
		{ ACCELERATION    , false },
		{ FORCE           , false },
		{ ANGULAR_IMPULSE , false },
		{ POSITION        , false },
		{ MOMENT          , false },
		{ IMPULSE         , false },
		{ ANGULAR_VELOCITY, false }
	};

	std::map<PointType, bool> pointDebugEnabled {
		{ INFO_POINT    , false },
		{ CENTER_OF_MASS, false },
		{ INTERSECTION  , false },
	};

	struct PointColorPair {
		Vec3f color1;
		Vec3f color2;
	};

	std::map<VectorType, Vec3f> vectorColors {
		{ INFO_VEC        , Vec3f(0, 1, 0) },
		{ VELOCITY        , Vec3f(0, 0, 1) },
		{ ACCELERATION    , Vec3f(0, 1, 1) },
		{ FORCE           , Vec3f(1, 0, 0) },
		{ POSITION        , Vec3f(1, 1, 0) },
		{ MOMENT          , Vec3f(1, 0, 1) },
		{ IMPULSE         , Vec3f(0.5, 0.7, 1) },
		{ ANGULAR_VELOCITY, Vec3f(0.75) },
		{ ANGULAR_IMPULSE , Vec3f(0.8, 0.1, 0.4) }
	};

	std::map<PointType, PointColorPair> pointColors{
		{ INFO_POINT    , PointColorPair { Vec3f(1.0f, 0.5f, 0.0f), Vec3f(1.0f, 0.2f, 0.0f) }},
		{ CENTER_OF_MASS, PointColorPair { Vec3f(1.0f, 1.0f, 0.0f), Vec3f(0.0f, 0.0f, 0.0f) }},
		{ INTERSECTION  , PointColorPair { Vec3f(0.0f, 0.0f, 1.0f), Vec3f(0.0f, 1.0f, 0.0f) }},
	};

	AddableBuffer<float> visibleVectors(900);
	AddableBuffer<float> visiblePoints(1000);

	SphereColissionRenderMode colissionSpheresMode;
	ColTreeRenderMode colTreeRenderMode;

	bool renderPiesEnabled = false;
	int fieldIndex = 0;

	void toggleVectorType(VectorType type) {
		vectorDebugEnabled[type] = !vectorDebugEnabled[type];
	}

	void togglePointType(PointType type) {
		pointDebugEnabled[type] = !pointDebugEnabled[type];
	}

	std::string toString(std::chrono::nanoseconds time) {
		std::stringstream string;
		string.precision(4);
		string << time.count() * 0.000001f;
		string << "ms";
		return string.str();
	}

	size_t getTheoreticalNumberOfIntersections(size_t objectCount) {
		return (objectCount - 1) * objectCount / 2;
	}

	void updateVectorMesh(VectorMesh* vectorMesh, AppDebug::ColoredVector* data, size_t size) {
		visibleVectors.clear();

		for (size_t i = 0; i < size; i++) {
			const AppDebug::ColoredVector& vector = data[i];
			if (vectorDebugEnabled[vector.type]) {
				visibleVectors.add(vector.origin.x);
				visibleVectors.add(vector.origin.y);
				visibleVectors.add(vector.origin.z);
				visibleVectors.add(vector.vector.x);
				visibleVectors.add(vector.vector.y);
				visibleVectors.add(vector.vector.z);
				visibleVectors.add(vectorColors[vector.type].x);
				visibleVectors.add(vectorColors[vector.type].y);
				visibleVectors.add(vectorColors[vector.type].z);
			}
		}

		vectorMesh->update(visibleVectors.data, visibleVectors.size / 9);
	}

	void updatePointMesh(PointMesh* pointMesh, AppDebug::ColoredPoint* data, size_t size) {
		visiblePoints.clear();

		for (size_t i = 0; i < size; i++) {
			const AppDebug::ColoredPoint& point = data[i];
			if (pointDebugEnabled[point.type]) {
				Vec3f color1 = pointColors[point.type].color1;
				Vec3f color2 = pointColors[point.type].color2;

				visiblePoints.add(point.point.x);
				visiblePoints.add(point.point.y);
				visiblePoints.add(point.point.z);
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

	void addDebugField(Vec2 dimension, Font* font, const char* varName, std::string value, const char* unit) {
		std::stringstream string;
		string.precision(4);
		string << varName << ": " << value << unit;
		Path::text(font, string.str().c_str(), Vec2(-dimension.x / dimension.y * 0.99, (1 - fieldIndex * 0.05) * 0.95), Vec4(1, 1, 1, 1), 0.001);
		fieldIndex++;
	}

	void renderDebugFields() {
		GUI::batch->submit();
	}
}