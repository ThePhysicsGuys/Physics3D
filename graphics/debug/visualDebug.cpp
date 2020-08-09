#include "core.h"

#include "visualDebug.h"

#include "mesh/vectorMesh.h"
#include "mesh/pointMesh.h"

#include "font.h"
#include "batch/commandBatch.h"
#include "batch/guiBatch.h"
#include "path/path.h"
#include "gui/gui.h"

#include "threePhaseBuffer.h"
#include "batch/commandBatch.h"

#include <sstream>

namespace Graphics {

const char* const graphicsDebugLabels[] {
		"Update",
		"Skybox",
		"Vectors",
		"Lighting",
		"Wait For Lock",
		"Physicals",
		"Origin",
		"Picker",
		"Profiler",
		"Finalize",
		"Other"
};

BreakdownAverageProfiler<GraphicsProcess> graphicsMeasure(graphicsDebugLabels, 60);

namespace Debug {

std::map<::Debug::VectorType, bool> vectorDebugEnabled {
	{ ::Debug::INFO_VEC        , false },
	{ ::Debug::VELOCITY        , false },
	{ ::Debug::ACCELERATION    , false },
	{ ::Debug::FORCE           , false },
	{ ::Debug::ANGULAR_IMPULSE , false },
	{ ::Debug::POSITION        , false },
	{ ::Debug::MOMENT          , false },
	{ ::Debug::IMPULSE         , false },
	{ ::Debug::ANGULAR_VELOCITY, false }
};

std::map<::Debug::PointType, bool> pointDebugEnabled {
	{ ::Debug::INFO_POINT    , false },
	{ ::Debug::CENTER_OF_MASS, false },
	{ ::Debug::INTERSECTION  , false },
};

struct PointColorPair {
	Vec3f color1;
	Vec3f color2;
};

std::map<::Debug::VectorType, Vec3f> vectorColors {
	{ ::Debug::INFO_VEC        , Vec3f(0, 1, 0) },
	{ ::Debug::VELOCITY        , Vec3f(0, 0, 1) },
	{ ::Debug::ACCELERATION    , Vec3f(0, 1, 1) },
	{ ::Debug::FORCE           , Vec3f(1, 0, 0) },
	{ ::Debug::POSITION        , Vec3f(1, 1, 0) },
	{ ::Debug::MOMENT          , Vec3f(1, 0, 1) },
	{ ::Debug::IMPULSE         , Vec3f(0.5, 0.7, 1) },
	{ ::Debug::ANGULAR_VELOCITY, Vec3f(0.75, 0.75, 0.75) },
	{ ::Debug::ANGULAR_IMPULSE , Vec3f(0.8, 0.1, 0.4) }
};

std::map<::Debug::PointType, PointColorPair> pointColors {
	{ ::Debug::INFO_POINT    , PointColorPair { Vec3f(1.0f, 0.5f, 0.0f), Vec3f(1.0f, 0.2f, 0.0f) }},
	{ ::Debug::CENTER_OF_MASS, PointColorPair { Vec3f(1.0f, 1.0f, 0.0f), Vec3f(0.0f, 0.0f, 0.0f) }},
	{ ::Debug::INTERSECTION  , PointColorPair { Vec3f(0.0f, 0.0f, 1.0f), Vec3f(0.0f, 1.0f, 0.0f) }},
};

AddableBuffer<float> visibleVectors(900);
AddableBuffer<float> visiblePoints(1000);

SphereColissionRenderMode colissionSpheresMode;
ColTreeRenderMode colTreeRenderMode;

bool renderPiesEnabled = false;
int fieldIndex = 0;

void toggleVectorType(::Debug::VectorType type) {
	vectorDebugEnabled[type] = !vectorDebugEnabled[type];
}

void togglePointType(::Debug::PointType type) {
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

void updateVectorMesh(Graphics::VectorMesh* vectorMesh, AppDebug::ColoredVector* data, size_t size) {
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

void updatePointMesh(Graphics::PointMesh* pointMesh, AppDebug::ColoredPoint* data, size_t size) {
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

void addDebugField(Vec2 dimension, Graphics::Font* font, const char* varName, std::string value, const char* unit) {
	std::stringstream string;
	string.precision(4);
	string << varName << ": " << value << unit;
	Path::text(font, string.str(), 0.001, Vec2(-dimension.x / dimension.y * 0.99, (1 - fieldIndex * 0.05) * 0.95), Vec4(1, 1, 1, 1));
	fieldIndex++;
}
}

};