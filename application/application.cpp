#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gui/screen.h"

int main(void) {
	if (!initGL())
		return -1;

	World w = World();
	
	Screen screen = Screen(800, 640, &w);

	/* Init GLEW after creating a valid rendering context */
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		return -1;
	}

	/* Loop until the user closes the window */
	while (!screen.shouldClose()) {
		screen.refresh();
	}

	screen.~Screen();
	return 0;
}