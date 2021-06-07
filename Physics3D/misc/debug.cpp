#include "debug.h"

#include "../geometry/shape.h"
#include "../part.h"

#include "serialization/serialization.h"
#include "toString.h"

#include <fstream>
#include <chrono>
#include <sstream>

namespace P3D {
namespace Debug {
void(*logVecAction)(Position, Vec3, VectorType) = [](Position, Vec3, VectorType) {};
void(*logPointAction)(Position, PointType) = [](Position, PointType) {};
void(*logCFrameAction)(CFrame, CFrameType) = [](CFrame, CFrameType) {};
void(*logShapeAction)(const Polyhedron&, const GlobalCFrame&) = [](const Polyhedron&, const GlobalCFrame&) {};
void(*logAction)(const char*, std::va_list) = [](const char* format, std::va_list args) { vprintf(format, args); };
void(*logWarnAction)(const char*, std::va_list) = [](const char* format, std::va_list args) { std::cout << "WARN: ";  vprintf(format, args); };
void(*logErrorAction)(const char*, std::va_list) = [](const char* format, std::va_list args) { std::cout << "ERROR: ";  vprintf(format, args); };

void logVector(Position origin, Vec3 vec, VectorType type) { logVecAction(origin, vec, type); };
void logPoint(Position point, PointType type) { logPointAction(point, type); }
void logCFrame(CFrame frame, CFrameType type) { logCFrameAction(frame, type); };
void logShape(const Polyhedron& shape, const GlobalCFrame& location) { logShapeAction(shape, location); };
void log(const char* format, ...) {
	std::va_list args;
	va_start(args, format);
	logAction(format, args);
	va_end(args);
}
void logWarn(const char* format, ...) {
	std::va_list args;
	va_start(args, format);
	logWarnAction(format, args);
	va_end(args);
}
void logError(const char* format, ...) {
	std::va_list args;
	va_start(args, format);
	logErrorAction(format, args);
	va_end(args);
}

void setVectorLogAction(void(*logger)(Position origin, Vec3 vec, VectorType type)) { logVecAction = logger; };
void setPointLogAction(void(*logger)(Position point, PointType type)) { logPointAction = logger; }
void setCFrameLogAction(void(*logger)(CFrame frame, CFrameType type)) { logCFrameAction = logger; };
void setShapeLogAction(void(*logger)(const Polyhedron& shape, const GlobalCFrame& location)) { logShapeAction = logger; }
void setLogAction(void(*logger)(const char* format, std::va_list args)) { logAction = logger; }
void setLogWarnAction(void(*logger)(const char* format, std::va_list args)) { logWarnAction = logger; }
void setLogErrorAction(void(*logger)(const char* format, std::va_list args)) { logErrorAction = logger; }


void saveIntersectionError(const Part& first, const Part& second, const char* reason) {
	logWarn("First cframe: %s", str(first.getCFrame()).c_str());
	logWarn("Second cframe: %s", str(second.getCFrame()).c_str());

	std::ofstream file;
	std::stringstream name;
	name << "../";
	name << reason;
	name << ".nativeParts";
	file.open(name.str().c_str(), std::ios::binary | std::ios::out);

	SerializationSessionPrototype session;
	const Part* parts[2]{&first, &second};
	session.serializeParts(parts, 2, file);

	file.close();
}
};
};