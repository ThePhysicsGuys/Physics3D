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
	struct ColoredVec {
		Vec3 origin, vec;
		double color;
		ColoredVec(Vec3 origin, Vec3 vec, double color) : origin(origin), vec(vec), color(color) {}
	};

	SwappableBuffer<ColoredVec> buf(16);

	namespace Logging {
		void logVec(Vec3 origin, Vec3 vec, Debug::VecType type) {
			using namespace Debug;
			double color;
			switch (type) {
			case INFO: color = 0.15; break;
			case FORCE: color = 0.0; break;
			case MOMENT: color = 0.1; break;
			case IMPULSE: color = 0.7; break;
			case POSITION: color = 0.5; break;
			case VELOCITY: color = 0.3; break;
			case ANGULAR_VELOCITY: color = 0.75; break;
			}

			buf.add(ColoredVec(origin, vec, color));
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
	}

	void updateVecMesh(VectorMesh* mesh) {
		buf.lockRead();
		mesh->update((double*)buf.readData, buf.readSize);
		buf.unlockRead();
	}
}