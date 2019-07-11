#include "debug.h"


namespace Debug {
	void(*logVecAction)(Vec3, Vec3, VectorType) = [](Vec3, Vec3, VectorType) {};
	void(*logPointAction)(Vec3, PointType) = [](Vec3, PointType) {};
	void(*logCFrameAction)(CFrame, CFrameType) = [](CFrame, CFrameType) {};
	void(*logShapeAction)(const Shape&) = [](const Shape&) {};
	
	void logVector(Vec3 origin, Vec3 vec, VectorType type) { logVecAction(origin, vec, type); };
	void logPoint(Vec3 point, PointType type) { logPointAction(point, type); }
	void logCFrame(CFrame frame, CFrameType type) { logCFrameAction(frame, type); };
	void logShape(Shape shape) { logShapeAction(shape); };

	void setVectorLogAction(void(*logger)(Vec3 origin, Vec3 vec, VectorType type)) { logVecAction = logger; };
	void setPointLogAction(void(*logger)(Vec3 point, PointType type)) { logPointAction = logger; }
	void setCFrameLogAction(void(*logger)(CFrame frame, CFrameType type)) { logCFrameAction = logger; };
	void setShapeLogAction(void(*logger)(const Shape& shape)) { logShapeAction = logger; }
}
