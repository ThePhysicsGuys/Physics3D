#include "application.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <chrono>

#include "gui/screen.h"

#include "../util/Log.h"


#define TICKS_PER_SECOND 500.0

std::thread physicsThread;
bool running = true;

World world = World();


void startPhysics();

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

	startPhysics();

	/* Loop until the user closes the window */
	while (!screen.shouldClose()) {
		screen.refresh();
	}

	stop(0);
}



void startPhysics() {
	using namespace std::chrono;

	auto nextTarget = system_clock::now() + milliseconds(200);

	physicsThread = std::thread([]() {
		while (running) {

			world.tick(1/TICKS_PER_SECOND);

			std::this_thread::sleep_for(milliseconds(500));
		}
	});
}

void stop(int returnCode) {
	
	running = false;
	physicsThread.join();

	glfwTerminate();
	exit(returnCode);
}
