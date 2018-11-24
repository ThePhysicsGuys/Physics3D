#include "application.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <chrono>

#include "gui/screen.h"

#include "../util/Log.h"

#include "tickerThread.h"


#define TICKS_PER_SECOND 500.0

#define TICK_SKIP_TIME std::chrono::milliseconds(3000)


World world = World();

TickerThread physicsThread;

std::thread t;

void setupPhysics();

int main(void) {
	Log::init();

	if (!initGLFW()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		return -1;
	}

	Screen screen = Screen(800, 640, &world);

	if (!initGLEW()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		return -1;
	}

	setupPhysics();

	physicsThread.start();

	/* Loop until the user closes the window */
	while (!screen.shouldClose()) {
		screen.refresh();
	}

	stop(0);
}



void stop(int returnCode) {
	physicsThread.stop();

	glfwTerminate();
	exit(returnCode);
}

void setupPhysics() {
	physicsThread = TickerThread(TICKS_PER_SECOND, TICK_SKIP_TIME, []() {
		//Log::info("%.9f", physicsThread.getTPS());
		world.tick(1 / physicsThread.getTPS());
	});
}
