#include "screen.h"
#include "../../util/log.h"
#include "../engine/math/vec2.h"
#include "../engine/math/mat4.h"
#include "shader.h"
#include "indexedMesh.h"
#include "mesh.h"
#include "../engine/geometry/shape.h"
#include "camera.h"
#include "../standardInputHandler.h"
#include "../engine/geometry/shape.h"
#include "../engine/geometry/boundingBox.h"

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

	/* Debug */
	// glEnable(GL_DEPTH_TEST);
}

Shader shader;

const unsigned int vertexCount1 = 4;
const unsigned int vertexCount2 = 6;
const unsigned int triangleCount = 2;

const Vec3 vertices1[vertexCount1] {
	Vec3(-0, -0, 0),
	Vec3( 1, -0, 0),
	Vec3( 1,  1, 0),
	Vec3(-0,  1, 0)
};

const double vertices2[vertexCount2 * 3]{
	-0.5, -0.5, -1,
	 0.5, -0.5, -1,
	 0.5,  0.5, -1,
	 0.5,  0.5, -1,
	-0.5,  0.5, -1,
	-0.5, -0.5, -1,
};

const Triangle triangles[triangleCount] = {
	{ 0, 1, 2 },
	{ 2, 3, 0 }
};

IndexedMesh* mesh1 = nullptr;
Mesh* mesh2 = nullptr;
BoundingBox* box = nullptr;
StandardInputHandler* handler = nullptr;
Shape shape(vertices1, triangles, vertexCount1, triangleCount);
Camera camera;

void Screen::init() {
	ShaderSource shaderSource = parseShader("../res/shaders/basic.shader");
	shader = Shader(shaderSource);
	shader.bind();
	camera.setPosition(0, 0, 4);

	shader.createUniform("viewMatrix");
	shader.createUniform("projectionMatrix");

	handler = new StandardInputHandler(window, &camera);

	box = new BoundingBox{-0.5, -0.5, -0.5, 0.5, 0.5, 0.5};
	
	mesh1 = new IndexedMesh(box->toShape(new Vec3[8]));
	mesh2 = new Mesh(vertices2, vertexCount2);
}

void Screen::makeCurrent() {
	glfwMakeContextCurrent(this->window);
}

void Screen::update() {
	if (handler->anyKey) {
		if (handler->getKey(GLFW_KEY_W)) {
			camera.move(0, 0, -1);
		}
		if (handler->getKey(GLFW_KEY_S)) {
			camera.move(0, 0, 1);
		}
		if (handler->getKey(GLFW_KEY_D)) {
			camera.move(1, 0, 0);
		}
		if (handler->getKey(GLFW_KEY_A)) {
			camera.move(-1, 0, 0);
		}
		if (handler->getKey(GLFW_KEY_SPACE)) {
			camera.move(0, 1, 0);
		}
		if (handler->getKey(GLFW_KEY_LEFT_SHIFT)) {
			camera.move(0, -1, 0);
		}
		if (handler->getKey(GLFW_KEY_LEFT)) {
			camera.rotate(0, -1, 0);
		}
		if (handler->getKey(GLFW_KEY_RIGHT)) {
			camera.rotate(0, 1, 0);
		}
		if (handler->getKey(GLFW_KEY_UP)) {
			camera.rotate(-1, 0, 0);
		}
		if (handler->getKey(GLFW_KEY_DOWN)) {
			camera.rotate(1, 0, 0);
		}
	}
	
}

int width;
int height;

void Screen::refresh() {
	/* Render here */
	glClear(GL_COLOR_BUFFER_BIT);
	/* Use the default shader */
	shader.bind();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
	glfwGetWindowSize(window, &width, &height);
	Mat4f projectionMatrix = Mat4f().perspective(1.0, float(height) / width, 0.1, 1000.0);
	shader.setUniform("projectionMatrix", projectionMatrix);

	Mat4f viewMatrix = Mat4f().rotate(camera.rotation.x, 1, 0, 0).rotate(camera.rotation.y, 0, 1, 0).rotate(camera.rotation.z, 0, 0, 1).translate(-camera.position.x, -camera.position.y, -camera.position.z);
	shader.setUniform("viewMatrix", viewMatrix);

	/* Render the mesh */
	mesh1->render();
	//mesh2->render();

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
