#include "application.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <chrono>
#include <iostream>

#include "gui/screen.h"

#include "../util/log.h"

#include "tickerThread.h"

#include "../engine/geometry/shape.h"
#include "../engine/geometry/managedShape.h"
#include "../engine/part.h"

#define TICKS_PER_SECOND 500.0

#define TICK_SKIP_TIME std::chrono::milliseconds(3000)

// Test shape
Vec3 testShapeVecs[]{Vec3(0.0, 0.0, 0.0), Vec3(1.0, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Vec3(0.0, 0.0, 1.0)};
Triangle testShapeTriangles[]{{0,2,1},{0,3,2},{0,1,3},{1,2,3}};
Shape testShape(testShapeVecs, testShapeTriangles, 4, 4);

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

	Part trianglePart = createVisiblePart(testShape, 10.0, 0.7);
	Part boxPart = createVisiblePart(BoundingBox{-0.1, -0.7, -0.3, 0.1, 0.7, 0.3}.toShape(new Vec3[8]), 2.0, 0.7);

	Physical triangleThing(trianglePart, CFrame(Vec3(0.3, 0.7, 0.2), fromEulerAngles(0.3, 0.0, 0.0)), 10.0, Mat3(1.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 5.0));
	Physical box(boxPart, CFrame(Vec3(-0.3, -0.7, 0.2), fromEulerAngles(0.0, 0.0, 0.0)), 10.0, Mat3(1.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 5.0));

	triangleThing.velocity = Vec3(0.1, 0.0, 0.0);
	triangleThing.angularVelocity = Vec3(0.1, 0.5, 0.3);

	box.angularVelocity = Vec3(1.0, 1.0, 1.0);

	world.addObject(triangleThing);
	world.addObject(box);

	physicsThread.start();

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

void setupPhysics() {
	physicsThread = TickerThread(TICKS_PER_SECOND, TICK_SKIP_TIME, []() {
		world.tick(1 / physicsThread.getTPS());
	});
}

Part createVisiblePart(Shape s, double density, double friction) {
	int id = screen.addMeshShape(s);

	Part p(s, density, friction);
	p.drawMeshId = id;
	return p;
}
