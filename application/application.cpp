#include "application.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <chrono>
#include <iostream>

#include "gui/screen.h"

#include "../util/Log.h"

#include "tickerThread.h"

#include "../engine/geometry/shape.h"

#define TICKS_PER_SECOND 500.0

#define TICK_SKIP_TIME std::chrono::milliseconds(3000)

// Test shape
Vec3 testShapeVecs[]{ Vec3(0.0, 0.0, 0.0), Vec3(1.0, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Vec3(0.0, 0.0, 1.0) };
Triangle testShapeTriangles[]{ {1,2,3}, {1,3,4}, {1, 4, 2}, {2, 3, 4} };
Shape testShape(testShapeVecs, 4, testShapeTriangles, 4);

Screen screen;
World world;

TickerThread physicsThread;

void init();
void setupPhysics();


int main(void) {
	init();

	/* Loop until the user closes the window */
	Log::info("Started rendering");
	while (!screen.shouldClose()) {
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
