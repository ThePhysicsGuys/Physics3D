#include "application.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <chrono>
#include <iostream>

#include "gui/screen.h"

#include "../util/Log.h"

#include "tickerThread.h"


#define TICKS_PER_SECOND 500.0

#define TICK_SKIP_TIME std::chrono::milliseconds(3000)


World world = World();

TickerThread physicsThread;

void setupPhysics();

int main(void) {
	Log::init();

	if (!initGLFW()) {
		Log::error("GLFW not initialised");
		std::cin.get();
		return -1;
	}

	Screen screen = Screen(800, 640, &world);

	if (!initGLEW()) {
		Log::error("GLEW not initialised");
		std::cin.get();
		return -1;
	}

	setupPhysics();

	screen.init();

	/* Loop until the user closes the window */
	while (!screen.shouldClose()) {
		screen.refresh();
	}

	screen.close();

	stop(0);
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
