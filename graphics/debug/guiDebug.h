#pragma once

#include "../physics/debug.h"
#include "../util/log.h"

#include "threePhaseBuffer.h"

#include <stdexcept>

namespace Graphics {

#ifdef _MSC_VER
#define ASSERT(x) if (!(x)) __debugbreak();
#else
#define ASSERT(x) if (!(x)) throw std::logic_error("Assert failed")
#endif
#define glCall(x) {Graphics::clearError(); x; ASSERT(Graphics::logCall(#x, __FILE__, __LINE__));}

void clearError();
bool logCall(const char* func, const char* file, int line);

namespace AppDebug {
struct ColoredVector {
	Vec3 origin, vector;
	::Debug::VectorType type;
	ColoredVector() = default;
	ColoredVector(Vec3 origin, Vec3 vector, ::Debug::VectorType type) : origin(origin), vector(vector), type(type) {}
	ColoredVector(Position origin, Vec3 vector, ::Debug::VectorType type) : origin(origin.x, origin.y, origin.z), vector(vector), type(type) {}
};
struct ColoredPoint {
	Vec3 point;
	::Debug::PointType type;
	ColoredPoint() = default;
	ColoredPoint(Vec3 point, ::Debug::PointType type) : point(point), type(type) {}
	ColoredPoint(Position point, ::Debug::PointType type) : point(point.x, point.y, point.z), type(type) {}
};

void setupDebugHooks();

void logTickStart();
void logTickEnd();

void logFrameStart();
void logFrameEnd();

AddableBuffer<ColoredVector>& getVectorBuffer();
AddableBuffer<ColoredPoint>& getPointBuffer();
}

};