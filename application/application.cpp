#include "application.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <chrono>

#include "gui/screen.h"

#include "../util/Log.h"

#include <iostream>


#define TICKS_PER_SECOND 500.0

#define TICK_SKIP_TIME seconds(3)

std::thread physicsThread;
bool running = true;

World world = World();


void startPhysics();

int main(void) {
	Log::init();

	if (!initGLFW()) {
		std::cin.get();
		return -1;
	}

	Screen screen = Screen(800, 640, &world);

	if (!initGLEW()) {
		std::cin.get();
		return -1;
	}
	
	startPhysics();

	screen.init();

	/* Loop until the user closes the window */
	while (!screen.shouldClose()) {
		screen.refresh();
	}

	screen.close();

	stop(0);
}

void startPhysics() {
	using namespace std::chrono;

	physicsThread = std::thread([]() {
		auto tickTime = microseconds((long long) (1000000 / TICKS_PER_SECOND));

		auto nextTarget = system_clock::now();

		while (running) {
			
			world.tick(1/TICKS_PER_SECOND);

			nextTarget += tickTime;
			auto curTime = system_clock::now();
			if (curTime < nextTarget) {
				std::this_thread::sleep_until(nextTarget);
			}else{
				// We're behind schedule
				if (nextTarget < curTime - TICK_SKIP_TIME) {
					Log::warn("Can't keep up! Skipping ticks!");

					nextTarget = curTime;
				}
			}
		}
	});
}

void stop(int returnCode) {
	
	running = false;
	physicsThread.join();

	glfwTerminate();
	exit(returnCode);
}
