#include "application.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <chrono>
#include <iostream>

#include "gui/screen.h"

#include "../util/log.h"

#include "tickerThread.h"

#include "objectLibrary.h"

#include "../engine/geometry/shape.h"
#include "../engine/geometry/managedShape.h"
#include "../engine/part.h"

#include "debug.h"

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

	Part trianglePart = createVisiblePart(triangleShape, 10.0, 0.7);
	Part boxPart = createVisiblePart(BoundingBox{-0.1, -0.7, -0.3, 0.1, 0.7, 0.3}.toShape(new Vec3[8]), 2.0, 0.7);
	Part icosathingie = createVisiblePart(Shape(newVerts, icosahedron.triangles, 12, 20), 10, 0.7);

	Part box2Part = createVisiblePart(BoundingBox{-10, -0.3, -0.3, 10, 0.3, 0.3}.toShape(new Vec3[8]), 1.0, 0.0);

	Physical triangleThing(trianglePart, CFrame(Vec3(0.0, 2.0, 0.2), fromEulerAngles(0.3, 0.0, 0.0)), Mat3(1.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 5.0));
	Physical box(boxPart, CFrame(Vec3(-0.3, -0.7, 0.2), fromEulerAngles(0.0, 0.0, 0.0)), Mat3(1.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 5.0));
	Physical icosaPhysical(icosathingie, CFrame(Vec3(0.0, 0.0, 0.0), fromEulerAngles(0.0, 0.0, 0.0)), Mat3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0));

	icosaPhysical.angularVelocity = Vec3(0.0, 1.5, 0.0);

	Physical box2Physical = Physical(box2Part, CFrame(Vec3(0.0, 2.0, 0.0), Mat3()), Mat3(100.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0));

	box2Physical.angularVelocity = Vec3(0.0, 0.0, 1.0);
	
	/*for (int x = 0; x < 10; x++) {
		// for (int y = 0; y < 10; y++) {
			for (int z = 0; z < 10; z++) {
				Physical phy(icosathingie, CFrame(Vec3(x, z, z) * 4, fromEulerAngles(0.0, 0.0, 0.0)), Mat3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0) * 5);
				world.addObject(phy);
			}
		// }
	}*/

	Part cubePart = createVisiblePart(BoundingBox{-0.5, -0.5, -0.5, 0.5, 0.5, 0.5}.toShape(new Vec3[8]), 1.0, 0.0);
	Physical cube(cubePart, CFrame(Vec3(0.5, 0.9, 0.5), fromEulerAngles(0.5, 0.2, 0.3)), Mat3());
	world.addObject(cube);

	// world.addObject(box2Physical);

	triangleThing.velocity = Vec3(1.0, 0.0, 0.0);
	triangleThing.angularVelocity = Vec3(0.1, 0.5, 0.3);

	box.angularVelocity = Vec3(1.0, 1.0, 1.0);

	// world.addObject(triangleThing);
	// world.addObject(box);
	// world.addObject(icosaPhysical);

	// physicsThread.start();

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
