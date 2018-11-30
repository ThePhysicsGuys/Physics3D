#include "screen.h"
#include "../../util/log.h"
#include "../engine/math/vec2.h"
#include "shader.h"
#include "indexedMesh.h"
#include "mesh.h"
#include "../engine/geometry/shape.h"
#include "camera.h"

#include <stdlib.h>

World* curWorld = NULL;

bool initGLFW() {
	/* Initialize the library */
	if (!glfwInit()) {
		Log::error("GLFW Failed to initialize!");
		return false;
	}

	return true;
}

bool initGLEW() {
	/* Init GLEW after creating a valid rendering context */
	if (glewInit() != GLEW_OK) {
		glfwTerminate();

		Log::error("GLEW Failed to initialize!");

		return false;
	}

	return true;
}

void terminateGL() {
	glfwTerminate();
}

int screenWidth;
int screenHeight;

Screen::Screen(int width, int height, World* w) {
	setWorld(w);

	screenWidth = width;
	screenHeight = height;

	/* Create a windowed mode window and its OpenGL context */
	this->window = glfwCreateWindow(width, height, "Physics3D", NULL, NULL);
	if (!this->window) {
		glfwTerminate();
		exit(-1);
	}

	/* Make the window's context current */
	makeCurrent();
}

Shader shader;

const unsigned int vertexCount = 9;

double vertices[vertexCount] = {
	-0.5, -0.5, 0,
	-0.5,  0.5, 0,
	0.5,  0.5, 0
};

Mesh* mesh = nullptr;
Camera camera;

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
	shader.setUniform("vResolution", Vec2(width, height));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Z) {
		Log::debug("x");
		camera.move(0.01, 0, 0);
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		camera.move(-0.01, 0, 0);
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		camera.move(0, 0.01, 0);
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		camera.move(0, -0.01, 0);

}

void Screen::init() {
	ShaderSource shaderSource = parseShader("../res/shaders/basic.shader");
	shader = Shader(shaderSource);
	shader.bind();

	mesh = new Mesh(vertices, vertexCount);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	shader.createUniform("vResolution");
	shader.createUniform("projectionMatrix");
	shader.createUniform("modelViewMatrix");
}

void Screen::makeCurrent() {
	glfwMakeContextCurrent(this->window);
}

void Screen::refresh() {
	/* Render here */
	glClear(GL_COLOR_BUFFER_BIT);

	/* Use the default shader */
	shader.bind();

	Mat4 projectionMatrix = Mat4();
	projectionMatrix = projectionMatrix.perspective(60 / 180 * 3.1415, screenWidth / screenHeight, 0.1, 1000);
	shader.setUniform("projectionMatrix", projectionMatrix);

	Mat4 modelViewMatrix = Mat4();
	modelViewMatrix.rotate(camera.rotation.x, 1, 0, 0).rotate(camera.rotation.y, 0, 1, 0).translate(camera.position.x, camera.position.y, camera.position.z);
	shader.setUniform("modelViewMatrix", modelViewMatrix);

	/* Render the mesh */
	mesh->render();

	/* Swap front and back buffers */
	glfwSwapBuffers(this->window);

	/* Poll for and process events */
	glfwPollEvents();
}

void Screen::close() {
	shader.close();
	mesh->close();
	terminateGL();
}

bool Screen::shouldClose() {
	return glfwWindowShouldClose(window);
}
