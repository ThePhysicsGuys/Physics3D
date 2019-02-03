#include "application.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <istream>

#include "gui/screen.h"

#include "../util/log.h"

#include "tickerThread.h"

#include "gui\loader.h"
#include "resourceManager.h"
#include "objectLibrary.h"
#include "../engine/geometry/shape.h"
#include "../engine/geometry/managedShape.h"
#include "../engine/part.h"
#include "../engine/world.h"
#include "worlds.h"

#include "debug.h"

#include "../engine/math/mathUtil.h"

#define TICKS_PER_SECOND 500.0

#define TICK_SKIP_TIME std::chrono::milliseconds(3000)


Screen screen;
GravityFloorWorld world(Vec3(0.0, -10.0, 0.0));

TickerThread physicsThread;

void init();
void setupPhysics();
Part createVisiblePart(NormalizedShape s, CFrame position, double density, double friction);
Part createVisiblePart(Shape s, CFrame position, double density, double friction);

/*void debugCallback(unsigned  source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, const void* userParam) {
	if (type == GL_DEBUG_TYPE_ERROR) 
		Log::error("(type 0x%x) %s", type, message);
	else
		Log::warn("(type=0x%x) %s", type, message);
}*/

int main(void) {
	init();


	/*Vec3 newVerts[12];
	for(int i = 0; i < 12; i++) {
		newVerts[i] = icosahedron.vertices[i] * 2;
	}*/

	// world.addObject(createVisiblePart(createCube(1.0), CFrame(Vec3(0.5, 1.9, 0.5), fromEulerAngles(1.5, 0.2, 0.3)), 1.0, 0.0));

	Part boxPart = createVisiblePart(BoundingBox{-0.1, -0.7, -0.3, 0.1, 0.7, 0.3}.toShape(new Vec3[8]), CFrame(Vec3(1.5, 0.7, 0.3), fromEulerAngles(0.0, 0.2, 0.0)), 2.0, 0.7);
	world.addObject(boxPart);



	world.addObject(createVisiblePart(createBox(0.7, 0.2, 0.7), CFrame(Vec3(0, 0.2, 0.0), fromEulerAngles(0.0, 0.0, 0.0)), 2.0, 0.7));
	world.addObject(createVisiblePart(createBox(0.5, 0.15, 0.5), CFrame(Vec3(0, 0.4, 0.0), fromEulerAngles(0.0, 0.0, 0.0)), 2.0, 0.7));
	world.addObject(createVisiblePart(createBox(0.3, 0.1, 0.3), CFrame(Vec3(0, 0.6, 0.0), fromEulerAngles(0.0, 0.0, 0.0)), 2.0, 0.7));


	Part icosaPart = createVisiblePart(icosahedron, CFrame(Vec3(0.0, 2.0, 3.0), fromEulerAngles(0.1, 0.1, 0.1)), 10, 0.7);
	world.addObject(icosaPart);

	/*Part veryLongBoxPart = createVisiblePart(BoundingBox{-0.1, -10, -0.1, 0.1, 10, 0.1}.toShape(new Vec3[8]), CFrame(Vec3(0.0, 10.0, 0.0), rotX(0.001)), 1.0, 1.0);
	world.addObject(veryLongBoxPart);*/

	Part housePart = createVisiblePart(house, CFrame(Vec3(-1.5, 1.0, 0.0), fromEulerAngles(0.7, 0.9, 0.7)), 1.0, 0.0);
	world.addObject(housePart);

	/*std::istringstream s = std::istringstream(getResourceAsString(STALL_MODEL));
	Shape stallShape = loadMesh(s);
	Part stallPart = createVisiblePart(stallShape, CFrame(Vec3(-2.0, 10.0, -2.0), fromEulerAngles(0.9, 0.1, 0.5)), 2, 0.7);
	world.addObject(stallPart);*/
	// Log::fatal("Is the stall Valid? %s", (stallPart.hitbox.isValid())? "yes" : "No :(");

	Part trianglePart = createVisiblePart(triangleShape, CFrame(Vec3(-2.0, 1.0, -2.0)), 10.0, 0.7);
	world.addObject(trianglePart);

	/*Vec3 vertss[]{Vec3(0,0,0), Vec3(1,0,0), Vec3(0,1,0)};
	Triangle triangless[]{Triangle{0,1,2}};

	Shape singleTriangle(vertss, triangless, 3, 1);

	Part specialTriangle = createVisiblePart(singleTriangle, 1.0, 1.0);
	Physical t(specialTriangle, CFrame());
	world.addObject(t);*/

	CFrame rotation = CFrame(fromEulerAngles(0.0, 0.5, 0.0));

	for(Physical& p: world.physicals) {
		p.part.cframe = rotation.localToGlobal(p.part.cframe);
	}

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

bool isPaused() {
	return paused;
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

Part createVisiblePart(NormalizedShape s, CFrame position, double density, double friction) {
	int id = screen.addMeshShape(s);

	Part p(s, position, density, friction);
	p.drawMeshId = id;
	return p;
}
Part createVisiblePart(Shape s, CFrame position, double density, double friction) {
	Part p(s, position, density, friction);

	int id = screen.addMeshShape(p.hitbox);

	p.drawMeshId = id;
	return p;
}
