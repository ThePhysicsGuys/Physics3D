#include "debug.h"

#include <GL/glew.h>

#include "../util/log.h"

#include "../engine/debug.h"
#include "../engine/math/mathUtil.h"
#include "threePhaseBuffer.h"

#include "view/screen.h"


void clearError() {
	while (glGetError() != GL_NO_ERROR);
}

bool logCall(const char* func, const char* file, int line) {
	bool response = true;
	unsigned int error = glGetError();
	while (error != GL_NO_ERROR) {
		Log::error("[OpenGL error 0x%x] %s:%d at %s", error, file, line, func);
		error = glGetError();
		response = false;
	}
	return response;
}

namespace AppDebug {
	

	ThreePhaseBuffer<ColoredVec> vecBuf(256);
	ThreePhaseBuffer<ColoredPoint> pointBuf(256);


	namespace Logging {
		using namespace Debug;
		void logVec(Vec3 origin, Vec3 vec, VecType type) {
			vecBuf.add(ColoredVec(origin, vec, type));
		}
		void logVec(Vec3f origin, Vec3f vec, VecType type) {
			vecBuf.add(ColoredVec(Vec3(origin), Vec3(vec), type));
		}
		void logPoint(Vec3 point, PointType type) {
			pointBuf.add(ColoredPoint(point, type));
		}

		void logCFrame(CFrame frame, CFrameType type) {
			switch(type) {
			case OBJECT_CFRAME:{
				Vec3 pos = frame.position;
				RotMat3 rot = frame.rotation;
				// buf.add(ColoredVec(frame.position, rot * Vec3(1.0, 0.0, 0.0), 0.0));
				// buf.add(ColoredVec(frame.position, rot * Vec3(0.0, 1.0, 0.0), 0.3));
				// buf.add(ColoredVec(frame.position, rot * Vec3(0.0, 0.0, 1.0), 0.6));
			}case INERTIAL_CFRAME: {
				Vec3 pos = frame.position;
				RotMat3 rot = frame.rotation;
				// buf.add(ColoredVec(frame.position, rot * Vec3(1.0, 0.0, 0.0), 0.1));
				// buf.add(ColoredVec(frame.position, rot * Vec3(0.0, 1.0, 0.0), 0.4));
				// buf.add(ColoredVec(frame.position, rot * Vec3(0.0, 0.0, 1.0), 0.7));
			}}
		}

		void logShape(const Shape& shape) {
			for(int i = 0; i < shape.triangleCount; i++) {
				Triangle t = shape.triangles[i];
				for(int j = 0; j < 3; j++) {
					Debug::logVec(shape[t[j]], shape[t[(j + 1) % 3]] - shape[t[j]], Debug::INFO_VEC);
				}
			}
		}
	}

	void logTickStart() {

	}

	void logTickEnd() {
		vecBuf.pushWriteBuffer();
		pointBuf.pushWriteBuffer();
	}

	void logFrameStart() {

	}

	void logFrameEnd() {

	}

	void setupDebugHooks() {
		Log::info("Set up debug hooks!");
		Debug::setVecLogAction(Logging::logVec);
		Debug::setPointLogAction(Logging::logPoint);
		Debug::setCFrameLogAction(Logging::logCFrame);
		Debug::setShapeLogAction(Logging::logShape);
	}

	/*
		Returns a copy of the current vec buffer
	*/
	AddableBuffer<ColoredVec>& getVecBuffer() {
		return vecBuf.pullOutputBuffer();
	}

	/*
		Returns a copy of the current point buffer
	*/
	AddableBuffer<ColoredPoint>& getPointBuffer() {
		return pointBuf.pullOutputBuffer();
	}
}
