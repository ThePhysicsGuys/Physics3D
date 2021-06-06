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

namespace P3D::Graphics {

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

namespace VisualDebug {

std::map<P3D::Debug::VectorType, bool> vectorDebugEnabled {
	{ P3D::Debug::INFO_VEC        , false },
	{ P3D::Debug::VELOCITY        , false },
	{ P3D::Debug::ACCELERATION    , false },
	{ P3D::Debug::FORCE           , false },
	{ P3D::Debug::ANGULAR_IMPULSE , false },
	{ P3D::Debug::POSITION        , false },
	{ P3D::Debug::MOMENT          , false },
	{ P3D::Debug::IMPULSE         , false },
	{ P3D::Debug::ANGULAR_VELOCITY, false }
};

std::map<P3D::Debug::PointType, bool> pointDebugEnabled {
	{ P3D::Debug::INFO_POINT    , false },
	{ P3D::Debug::CENTER_OF_MASS, false },
	{ P3D::Debug::INTERSECTION  , false },
};

struct PointColorPair {
	Vec3f color1;
	Vec3f color2;
};

std::map<P3D::Debug::VectorType, Vec3f> vectorColors {
	{ P3D::Debug::INFO_VEC        , Vec3f(0, 1, 0) },
	{ P3D::Debug::VELOCITY        , Vec3f(0, 0, 1) },
	{ P3D::Debug::ACCELERATION    , Vec3f(0, 1, 1) },
	{ P3D::Debug::FORCE           , Vec3f(1, 0, 0) },
	{ P3D::Debug::POSITION        , Vec3f(1, 1, 0) },
	{ P3D::Debug::MOMENT          , Vec3f(1, 0, 1) },
	{ P3D::Debug::IMPULSE         , Vec3f(0.5, 0.7, 1) },
	{ P3D::Debug::ANGULAR_VELOCITY, Vec3f(0.75, 0.75, 0.75) },
	{ P3D::Debug::ANGULAR_IMPULSE , Vec3f(0.8, 0.1, 0.4) }
};

std::map<P3D::Debug::PointType, PointColorPair> pointColors {
	{ P3D::Debug::INFO_POINT    , PointColorPair { Vec3f(1.0f, 0.5f, 0.0f), Vec3f(1.0f, 0.2f, 0.0f) }},
	{ P3D::Debug::CENTER_OF_MASS, PointColorPair { Vec3f(1.0f, 1.0f, 0.0f), Vec3f(0.0f, 0.0f, 0.0f) }},
	{ P3D::Debug::INTERSECTION  , PointColorPair { Vec3f(0.0f, 0.0f, 1.0f), Vec3f(0.0f, 1.0f, 0.0f) }},
};

AddableBuffer<float> visibleVectors(900);
AddableBuffer<float> visiblePoints(1000);

SphereColissionRenderMode colissionSpheresMode;
int colTreeRenderMode = -1; // -2 for selected, -1 for none, n >= 0 for layer tree n


bool renderPiesEnabled = false;
int fieldIndex = 0;

void toggleVectorType(P3D::Debug::VectorType type) {
	vectorDebugEnabled[type] = !vectorDebugEnabled[type];
}

void togglePointType(P3D::Debug::PointType type) {
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
	Path::text(font, string.str(), 0.001, Vec2(-dimension.x / dimension.y * 0.99, (1 - fieldIndex * 0.05) * 0.95), Color(1, 1, 1, 1));
	fieldIndex++;
}
}

};