#pragma once

#include <cstdarg>

#include "../math/linalg/vec.h"
#include "../math/position.h"
#include "../math/cframe.h"
#include "../math/globalCFrame.h"

namespace P3D {
class Part;

class Polyhedron;

namespace Debug {

enum VectorType {
	INFO_VEC,
	FORCE,
	MOMENT,
	IMPULSE,
	ANGULAR_IMPULSE,
	POSITION,
	VELOCITY,
	ACCELERATION,
	ANGULAR_VELOCITY,
};

enum PointType {
	INFO_POINT,
	CENTER_OF_MASS,
	INTERSECTION,
};

enum CFrameType {
	OBJECT_CFRAME,
	INERTIAL_CFRAME
};

void logVector(Position origin, Vec3 vec, VectorType type);
void logPoint(Position point, PointType type);
void logCFrame(CFrame frame, CFrameType type);
void logShape(const Polyhedron& shape, const GlobalCFrame& location);
void log(const char* format, ...);
void logWarn(const char* format, ...);
void logError(const char* format, ...);

void setVectorLogAction(void(*logger)(Position origin, Vec3 vec, VectorType type));
void setPointLogAction(void(*logger)(Position point, PointType type));
void setCFrameLogAction(void(*logger)(CFrame frame, CFrameType type));
void setShapeLogAction(void(*logger)(const Polyhedron& shape, const GlobalCFrame& location));
void setLogAction(void(*logger)(const char* format, std::va_list args));
void setLogWarnAction(void(*logger)(const char* format, std::va_list args));
void setLogErrorAction(void(*logger)(const char* format, std::va_list args));

void saveIntersectionError(const Part& first, const Part& second, const char* reason);
};
};