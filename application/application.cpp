#include "application.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

#include "gui/screen.h"

#include "../util/log.h"

#include "tickerThread.h"

#include "gui\loader.h"
#include "resourceManager.h"
#include "objectLibrary.h"
#include "../engine/geometry/shape.h"
#include "../engine/geometry/managedShape.h"
#include "../engine/part.h"

#include "debug.h"

#include "../engine/math/mathUtil.h"

#define TICKS_PER_SECOND 500.0

#define TICK_SKIP_TIME std::chrono::milliseconds(3000)


Screen screen;
World world;

TickerThread physicsThread;

void init();
void setupPhysics();
Part createVisiblePart(Shape s, double density, double friction);

/*void debugCallback(unsigned  source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, const void* userParam) {
	if (type == GL_DEBUG_TYPE_ERROR) 
		Log::error("(type 0x%x) %s", type, message);
	else
		Log::warn("(type=0x%x) %s", type, message);
}*/

int main(void) {
	init();


	Vec3 newVerts[12];
	for (int i = 0; i < 12; i++) {
		newVerts[i] = icosahedron.vertices[i] * 2;
	}

	Part stallPart = createVisiblePart(*loadMesh((std::istream&) std::istringstream(getResourceAsString(STALL_MODEL))), 1, 0.7);
	Part trianglePart = createVisiblePart(triangleShape, 10.0, 0.7);
	Part boxPart = createVisiblePart(BoundingBox{-0.1, -0.7, -0.3, 0.1, 0.7, 0.3}.toShape(new Vec3[8]), 2.0, 0.7);
	Part icosaPart = createVisiblePart(Shape(newVerts, icosahedron.triangles, 12, 20), 10, 0.7);
	Part cubePart = createVisiblePart(createCube(1.0), 1000.0, 0.0);
	Part housePart = createVisiblePart(house, 1000.0, 0.0);

	Physical cube(cubePart, CFrame(Vec3(0.5, 1.9, 0.5), fromEulerAngles(1.5, 0.2, 0.3)));
	Physical box(boxPart, CFrame(Vec3(1.5, 0.7, 0.3), fromEulerAngles(0.0, 0.2, 0.0)));
	Physical housePhysical(housePart, CFrame(Vec3(-1.5, 0.0, 0.0), fromEulerAngles(0.7, 0.9, 0.7)));
	Physical icosa(icosaPart, CFrame(Vec3(0.0, 2.0, 3.0), fromEulerAngles(0.1, 0.1, 0.1)));

	world.addObject(cube);
	world.addObject(box);
	world.addObject(housePhysical);
	world.addObject(icosa);

	/* Loop until the user closes the window */
	Log::info("Started rendering");
	while (!screen.shouldClose()) {
		screen.update();
		screen.refresh();
	}

	Log::info("Closing screen");
	screen.close();
	Log::info("Closed screen");

	stop(0);
}

void init() {
	AppDebug::setupDebugHooks();

	if (!initGLFW()) {
		Log::error("GLFW not initialised");
		std::cin.get();
		stop(-1);
	}

	screen = Screen(800, 640, &world);

	if (!initGLEW()) {
		Log::error("GLEW not initialised");
		std::cin.get();
		stop(-1);
	}

	setupPhysics();

	Log::info("Initializing screen");
	screen.init();
	Log::info("Initialized screen");

}

void stop(int returnCode) {
	physicsThread.stop();

	glfwTerminate();
	exit(returnCode);
}

bool paused = true;
void togglePause() {
	if (paused)
		unpause();
	else
		pause();
}

void pause() {
	physicsThread.stop();
	paused = true;
}

void unpause() {
	physicsThread.start();
	paused = false;
}

void setSpeed(double newSpeed) {
	physicsThread.setSpeed(newSpeed);
}

double getSpeed() {
	return physicsThread.getSpeed();
}

void runTick() {
	physicsThread.runTick();
}

void setupPhysics() {
	physicsThread = TickerThread(TICKS_PER_SECOND, TICK_SKIP_TIME, []() {
		AppDebug::logTickStart();
		world.tick(1 / physicsThread.getTPS());
		AppDebug::logTickEnd();
	});
}

Part createVisiblePart(Shape s, double density, double friction) {
	int id = screen.addMeshShape(s);

	Part p(s, density, friction);
	p.drawMeshId = id;
	return p;
}
