#include "screen.h"
#include "../../util/Log.h"
#include "shader.h"
#include "mesh.h"
#include "../engine/geometry/shape.h"

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

Screen::Screen(int width, int height, World* w) {
	setWorld(w);

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

const unsigned int vertexCount = 4;
const unsigned int triangleCount = 2;

Vec3 vertices[vertexCount] = {
	Vec3(-1, -1, 0),
	Vec3(-1,  1, 0),
	Vec3(1,  1, 0),
	Vec3(1, -1, 0)
};

Triangle triangles[triangleCount] = {
	{ 0, 1, 2 },
	{ 0, 2, 3 }
};

Shape shape(vertices, vertexCount, triangles, triangleCount);
Mesh* mesh = nullptr;

void Screen::init() {
	ShaderSource shaderSource = parseShader("../res/shaders/basic.shader");
	shader = Shader(shaderSource);
	mesh = &Mesh(shape);
}

void Screen::makeCurrent() {
	glfwMakeContextCurrent(this->window);
}

void Screen::refresh() {

	/* Render here */
	glClear(GL_COLOR_BUFFER_BIT);

	mesh->render();

	/* Swap front and back buffers */
	glfwSwapBuffers(this->window);

	/* Poll for and process events */
	glfwPollEvents();
}

void Screen::close() {
	shader.close();
	terminateGL();
}

bool Screen::shouldClose() {
	return glfwWindowShouldClose(window);
}
