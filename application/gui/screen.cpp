#include "screen.h"
#include "../../util/log.h"
#include "../engine/math/vec2.h"
#include "shader.h"
#include "indexedMesh.h"
#include "mesh.h"
#include "../engine/geometry/shape.h"
#include "camera.h"
#include "../standardInputHandler.h"
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

const unsigned int vertexCount1 = 4;
const unsigned int vertexCount2 = 6;
const unsigned int triangleCount = 2;

const double vertices1[vertexCount1 * 3] {
	-0, -0, 0,
	 1, -0, 0,
	 1,  1, 0,
	-0,  1, 0,
};

const double vertices2[vertexCount2 * 3]{
	-0.5, -0.5, 0,
	 0.5, -0.5, 0,
	 0.5,  0.5, 0,
	 0.5,  0.5, 0,
	-0.5,  0.5, 0,
	-0.5, -0.5, 0,
};

const unsigned int triangles[triangleCount * 3] = {
	0, 1, 2,
	2, 3, 0 
};

IndexedMesh* mesh1 = nullptr;
Mesh* mesh2 = nullptr;
StandardInputHandler* handler = nullptr;
//Shape shape(vertices, triangles, vertexCount, triangleCount);
Camera camera;

void Screen::init() {
	ShaderSource shaderSource = parseShader("../res/shaders/basic.shader");
	shader = Shader(shaderSource);
	shader.bind();

	handler = new StandardInputHandler(window, &camera);

	mesh1 = new IndexedMesh(vertices1, triangles, vertexCount1, triangleCount);
	mesh2 = new Mesh(vertices2, vertexCount2);
}

void Screen::makeCurrent() {
	glfwMakeContextCurrent(this->window);
}

void Screen::refresh() {
	/* Render here */
	glClear(GL_COLOR_BUFFER_BIT);

	/* Use the default shader */
	shader.bind();

	/* Render the mesh */
	mesh1->render();
	mesh2->render();

	/* Swap front and back buffers */
	glfwSwapBuffers(this->window);

	/* Poll for and process events */
	glfwPollEvents();
}

void Screen::close() {
	shader.close();
	//mesh->close();
	terminateGL();
}

bool Screen::shouldClose() {
	return glfwWindowShouldClose(window) != 0;
}
