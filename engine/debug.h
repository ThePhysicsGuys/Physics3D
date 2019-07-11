#pragma once

#include "math/vec3.h"
#include "math/cframe.h"
#include "geometry/shape.h"

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

	void logVector(Vec3 origin, Vec3 vec, VectorType type);
	inline void logVector(Vec3f origin, Vec3f vec, VectorType type) { logVector(Vec3(origin), Vec3(vec), type); };
	void logPoint(Vec3 point, PointType type);
	void logCFrame(CFrame frame, CFrameType type);
	void logShape(Shape shape);

	void setVectorLogAction(void(*logger)(Vec3 origin, Vec3 vec, VectorType type));
	void setPointLogAction(void(*logger)(Vec3 point, PointType type));
	void setCFrameLogAction(void(*logger)(CFrame frame, CFrameType type));
	void setShapeLogAction(void(*logger)(const Shape& shape));
}
