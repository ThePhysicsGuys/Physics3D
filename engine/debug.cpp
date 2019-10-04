#include "debug.h"

#include "geometry/shape.h"

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
}
