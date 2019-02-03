#include "debug.h"

#include "../util/log.h"
#include "../engine/debug.h"

#include "../engine/math/mathUtil.h"

#include "GL\glew.h"

#include "gui/screen.h"

#include "buffers.h"

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
	

	SwappableBuffer<ColoredVec> buf(16);

	namespace Logging {
		using namespace Debug;
		void logVec(Vec3 origin, Vec3 vec, VecType type) {
			/*if(!debug_enabled[type]) return;
			double color;
			switch (type) {
			case INFO: color = 0.15; break;
			case FORCE: color = 0.0; break;
			case MOMENT: color = 0.1; break;
			case IMPULSE: color = 0.7; break;
			case POSITION: color = 0.5; break;
			case VELOCITY: color = 0.3; break;
			case ANGULAR_VELOCITY: color = 0.75; break;
			}*/

			// Log::debug("Added vec %s, %s, %d", str(origin).c_str(), str(vec).c_str(), type);

			buf.add(ColoredVec(origin, vec, type));
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
	}

	void logTickStart() {

	}

	void logTickEnd() {
		buf.swap();
	}

	void setupDebugHooks() {
		Log::info("Set up debug hooks!");
		Debug::setVecLogAction(Logging::logVec);
		Debug::setCFrameLogAction(Logging::logCFrame);
	}

	/*
		Returns a copy of the current vec buffer
	*/
	AddableBuffer<ColoredVec> getVecBuffer() {
		return buf.getReadBuffer();
	}
}
