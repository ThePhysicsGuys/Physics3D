#include "debug.h"

#include "../util/log.h"
#include "../engine/debug.h"

#include "../engine/math/mathUtil.h"

#include "GL\glew.h"

#include "gui/screen.h"

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

size_t vecBufSize = 512;
double * vecBuf = (double*) malloc(sizeof(double)*7);
size_t vecBufIndex = 0;

namespace Logging {
	void logVec(Vec3 origin, Vec3 vec) {
		Log::debug("@%s: %s", str(origin).c_str(), str(vec).c_str());
		
		vecBuf[vecBufIndex++] = origin.x;
		vecBuf[vecBufIndex++] = origin.y;
		vecBuf[vecBufIndex++] = origin.z;
		vecBuf[vecBufIndex++] = vec.x;
		vecBuf[vecBufIndex++] = vec.y;
		vecBuf[vecBufIndex++] = vec.z;
		vecBuf[vecBufIndex++] = 0.3;

		Log::warn("vecBufIndex: %d", vecBufIndex);

		if (vecBufIndex >= vecBufSize) {
			double* newBuf = (double*) realloc(vecBuf, vecBufSize *= 2);
		}
	}
}

void logTickStart() {
	vecBufIndex = 0;
}

void logTickEnd() {
	getVectorMesh()->update(vecBuf, vecBufIndex);
}

void setupDebugHooks() {
	Log::info("Set up debug hooks!");
	setVecLogAction(Logging::logVec);
}
