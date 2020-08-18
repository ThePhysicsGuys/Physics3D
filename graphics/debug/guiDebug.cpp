#include "core.h"

#include "guiDebug.h"

#include <GL/glew.h>

#include "../util/log.h"
#include "../physics/geometry/shape.h"
#include "../physics/debug.h"
#include "../physics/math/mathUtil.h"
#include "threePhaseBuffer.h"

#include "../physics/geometry/polyhedron.h"

namespace P3D::Graphics {

void renderQuad() {
	static unsigned int VAO = 0;
	static unsigned int VBO;
	if (VAO == 0) {
		float vertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
	}
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

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

ThreePhaseBuffer<ColoredVector> vecBuf(256);
ThreePhaseBuffer<ColoredPoint> pointBuf(256);

namespace Logging {
using namespace Debug;

void logVector(Position origin, Vec3 vec, VectorType type) {
	vecBuf.add(ColoredVector(origin, vec, type));
}

void logPoint(Position point, PointType type) {
	pointBuf.add(ColoredPoint(point, type));
}

void logCFrame(CFrame frame, CFrameType type) {
	switch (type) {
		case OBJECT_CFRAME: {
				Vec3 pos = frame.position;
				Rotation rot = frame.rotation;
				// buf.add(ColoredVec(frame.position, rot * Vec3(1.0, 0.0, 0.0), 0.0));
				// buf.add(ColoredVec(frame.position, rot * Vec3(0.0, 1.0, 0.0), 0.3));
				// buf.add(ColoredVec(frame.position, rot * Vec3(0.0, 0.0, 1.0), 0.6));
			}
						  break;
		case INERTIAL_CFRAME: {
				Vec3 pos = frame.position;
				Rotation rot = frame.rotation;
				// buf.add(ColoredVec(frame.position, rot * Vec3(1.0, 0.0, 0.0), 0.1));
				// buf.add(ColoredVec(frame.position, rot * Vec3(0.0, 1.0, 0.0), 0.4));
				// buf.add(ColoredVec(frame.position, rot * Vec3(0.0, 0.0, 1.0), 0.7));
			}
							break;
	}
}

void logShape(const Polyhedron& shape, const GlobalCFrame& location) {
	for (int i = 0; i < shape.triangleCount; i++) {
		Triangle t = shape.getTriangle(i);
		for (int j = 0; j < 3; j++) {
			Debug::logVector(location.localToGlobal(shape.getVertex(t[j])), location.localToRelative(shape.getVertex(t[(j + 1) % 3]) - shape.getVertex(t[j])), Debug::INFO_VEC);
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
	Debug::setVectorLogAction(Logging::logVector);
	Debug::setPointLogAction(Logging::logPoint);
	Debug::setCFrameLogAction(Logging::logCFrame);
	Debug::setShapeLogAction(Logging::logShape);
}

/*
	Returns a copy of the current vec buffer
*/
AddableBuffer<ColoredVector>& getVectorBuffer() {
	return vecBuf.pullOutputBuffer();
}

/*
	Returns a copy of the current point buffer
*/
AddableBuffer<ColoredPoint>& getPointBuffer() {
	return pointBuf.pullOutputBuffer();
}
}

};