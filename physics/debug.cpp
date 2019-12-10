#include "debug.h"

#include "geometry/shape.h"
#include "part.h"
#include "misc/serialization.h"

#include <fstream>
#include <chrono>
#include <sstream>

namespace Debug {
	void(*logVecAction)(Position, Vec3, VectorType) = [](Position, Vec3, VectorType) {};
	void(*logPointAction)(Position, PointType) = [](Position, PointType) {};
	void(*logCFrameAction)(CFrame, CFrameType) = [](CFrame, CFrameType) {};
	void(*logShapeAction)(const Polyhedron&, const GlobalCFrame&) = [](const Polyhedron&, const GlobalCFrame&) {};
	
	void logVector(Position origin, Vec3 vec, VectorType type) { logVecAction(origin, vec, type); };
	void logPoint(Position point, PointType type) { logPointAction(point, type); }
	void logCFrame(CFrame frame, CFrameType type) { logCFrameAction(frame, type); };
	void logShape(const Polyhedron& shape, const GlobalCFrame& location) { logShapeAction(shape, location); };

	void setVectorLogAction(void(*logger)(Position origin, Vec3 vec, VectorType type)) { logVecAction = logger; };
	void setPointLogAction(void(*logger)(Position point, PointType type)) { logPointAction = logger; }
	void setCFrameLogAction(void(*logger)(CFrame frame, CFrameType type)) { logCFrameAction = logger; };
	void setShapeLogAction(void(*logger)(const Polyhedron& shape, const GlobalCFrame& location)) { logShapeAction = logger; }


	void saveIntersectionError(const Part* first, const Part* second, const char* reason) {
		std::ofstream file;
		std::stringstream name;
		name << reason;
		name << ".looseParts";
		file.open(name.str().c_str(), std::ios::binary | std::ios::out);

		serialize<int>(2, file);
		serialize<Part>(*first, file);
		serialize<Part>(*second, file);

		file.close();
	}
}
