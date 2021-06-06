#pragma once

#include <Physics3D/misc/debug.h>
#include "../util/log.h"

#include "threePhaseBuffer.h"

#include <stdexcept>

namespace P3D::Graphics {

#define ASSERT(x) if (!(x)) throw std::logic_error("Assert failed")
#define glCall(x) {Graphics::clearError(); x; ASSERT(Graphics::logCall(#x, __FILE__, __LINE__));}

void clearError();
bool logCall(const char* func, const char* file, int line);

namespace AppDebug {
struct ColoredVector {
	Vec3 origin, vector;
	P3D::Debug::VectorType type;
	ColoredVector() = default;
	ColoredVector(Vec3 origin, Vec3 vector, P3D::Debug::VectorType type) : origin(origin), vector(vector), type(type) {}
	ColoredVector(Position origin, Vec3 vector, P3D::Debug::VectorType type) : origin(castPositionToVec3(origin)), vector(vector), type(type) {}
};
struct ColoredPoint {
	Vec3 point;
	P3D::Debug::PointType type;
	ColoredPoint() = default;
	ColoredPoint(Vec3 point, P3D::Debug::PointType type) : point(point), type(type) {}
	ColoredPoint(Position point, P3D::Debug::PointType type) : point(castPositionToVec3(point)), type(type) {}
};

void setupDebugHooks();

void logTickStart();
void logTickEnd();

void logFrameStart();
void logFrameEnd();

void renderQuad();

AddableBuffer<ColoredVector>& getVectorBuffer();
AddableBuffer<ColoredPoint>& getPointBuffer();
}

};