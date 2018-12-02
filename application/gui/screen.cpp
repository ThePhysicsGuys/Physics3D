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

const unsigned int vertexCount = 4;
const unsigned int triangleCount = 2;

const double vertices[vertexCount * 3] {
	-0.5, -0.5, 0,
	 0.5, -0.5, 0,
	 0.5,  0.5, 0,
	-0.5,  0.5, 0
};

const unsigned int triangles[triangleCount * 3] = {
	0, 1, 2,
	2, 3, 0 
};

IndexedMesh* mesh = nullptr;
StandardInputHandler* handler = nullptr;
//Shape shape(vertices, triangles, vertexCount, triangleCount);
Camera camera;

unsigned int vao;
unsigned int vbo;
unsigned int ibo;

void Screen::init() {
	ShaderSource shaderSource = parseShader("../res/shaders/basic.shader");
	shader = Shader(shaderSource);
	shader.bind();

	handler = new StandardInputHandler(window, &camera);

	mesh = new IndexedMesh(vertices, triangles, vertexCount, triangleCount);
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
	mesh->render();

	/*glDisable(GL_CULL_FACE);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_INT, nullptr);*/

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
