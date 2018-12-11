#include "screen.h"

#include "shader.h"
#include "indexedMesh.h"
#include "arrayMesh.h"
#include "vectorMesh.h"
#include "camera.h"
#include "picker.h"
#include "../debug.h"
#include "../standardInputHandler.h"
#include "../resourceManager.h"

#include "../../util/log.h"

#include "../engine/math/vec2.h"
#include "../engine/math/mat4.h"
#include "../engine/geometry/shape.h"
#include "../engine/geometry/shape.h"
#include "../engine/geometry/boundingBox.h"
#include "../engine/math/mathUtil.h"

#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <math.h>

World* curWorld = NULL;
Vec2 screenSize;
Vec3 ray;

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

IndexedMesh* boxMesh = nullptr;
VectorMesh* vectorMesh = nullptr;
ArrayMesh* originMesh = nullptr;
IndexedMesh* transMesh = nullptr;

Shader basicShader;
Shader vectorShader;
Shader originShader;

BoundingBox* box = nullptr;
StandardInputHandler* handler = nullptr;
Camera camera;

void Screen::init() {
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	screenSize = Vec2(width, height);

	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);

	ShaderSource basicShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(BASIC_SHADER1)), "basic.shader");
	ShaderSource vectorShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(VECTOR_SHADER1)), "vector.shader");
	ShaderSource originShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(ORIGIN_SHADER1)), "origin.shader");

	basicShader = Shader(basicShaderSource);
	basicShader.createUniform("modelMatrix");
	basicShader.createUniform("viewMatrix");
	basicShader.createUniform("color");
	basicShader.createUniform("projectionMatrix");
	basicShader.createUniform("viewPosition");

	vectorShader = Shader(vectorShaderSource);
	vectorShader.createUniform("viewMatrix");
	vectorShader.createUniform("projectionMatrix");
	vectorShader.createUniform("viewPosition");


	originShader = Shader(originShaderSource);
	originShader.createUniform("viewMatrix");
	originShader.createUniform("projectionMatrix");
	originShader.createUniform("viewPosition");

	camera.setPosition(1, 1, -2);
	camera.setRotation(0.3, 3.1415, 0.0);

	handler = new StandardInputHandler(window, this, &camera);

	box = new BoundingBox{-0.5, -0.5, -0.5, 0.5, 0.5, 0.5};
	Shape shape = box->toShape(new Vec3[8]);// .rotated(fromEulerAngles(0.5, 0.1, 0.2), new Vec3[8]);

	boxMesh = new IndexedMesh(shape);

	Shape trans = shape.translated(Vec3(1.0, -1.0, 1.0), new Vec3[8]);
	transMesh = new IndexedMesh(trans);

	double originVertices[3] = { 0, 0, 0 };
	originMesh = new ArrayMesh(originVertices, 1, 3, RenderMode::POINTS);

	double * vecs = new double[140];

	vecs[0] = 0.2;
	vecs[1] = 0.3;
	vecs[2] = 0.7;
	vecs[3] = 0.8;
	vecs[4] = 0.6;
	vecs[5] = 0.7;
	vecs[6] = 0.5;

	vectorMesh = new VectorMesh(vecs, 20);

	
}

void Screen::makeCurrent() {
	glfwMakeContextCurrent(this->window);
}

void Screen::update() {
	if (handler->anyKey) {
		if (handler->getKey(GLFW_KEY_W)) {
			camera.move(0, 0, -10);
		}
		if (handler->getKey(GLFW_KEY_S)) {
			camera.move(0, 0, 10);
		}
		if (handler->getKey(GLFW_KEY_D)) {
			camera.move(10, 0, 0);
		}
		if (handler->getKey(GLFW_KEY_A)) {
			camera.move(-10, 0, 0);
		}
		if (handler->getKey(GLFW_KEY_SPACE)) {
			camera.move(0, 10, 0);
		}
		if (handler->getKey(GLFW_KEY_LEFT_SHIFT)) {
			camera.move(0, -10, 0);
		}
		if (handler->getKey(GLFW_KEY_LEFT)) {
			camera.rotate(0, -10, 0);
		}
		if (handler->getKey(GLFW_KEY_RIGHT)) {
			camera.rotate(0, 10, 0);
		}
		if (handler->getKey(GLFW_KEY_UP)) {
			camera.rotate(-10, 0, 0);
		}
		if (handler->getKey(GLFW_KEY_DOWN)) {
			camera.rotate(10, 0, 0);
		}
	}
}

void Screen::refresh() {
	AppDebug::updateVecMesh(vectorMesh);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_BUFFER);


	Mat4f projectionMatrix = Mat4f().perspective(1.0, screenSize.x / screenSize.y, 0.01, 100.0);
	Mat4f viewMatrix = Mat4f().rotate(camera.rotation.x, 1, 0, 0).rotate(camera.rotation.y, 0, 1, 0).rotate(camera.rotation.z, 0, 0, 1).translate(-camera.position.x, -camera.position.y, -camera.position.z);
	Vec3 realCameraVector = Mat4().rotate(camera.rotation.x, 1, 0, 0).rotate(camera.rotation.y, 0, 1, 0).rotate(camera.rotation.z, 0, 0, 1) * Vec3(0, 0, -1);

	basicShader.bind();
	basicShader.setUniform("color", Vec3f(1, 1, 1));
	basicShader.setUniform("projectionMatrix", projectionMatrix);
	basicShader.setUniform("viewMatrix", viewMatrix);
	basicShader.setUniform("viewPosition", Vec3f(camera.position.x, camera.position.y, camera.position.z));
	
	// Render world objects
	for (Physical p : w->physicals) {
		CFrame fra = p.cframe;
		Mat4 transf = fra.asMat4();

		Mat4f transff = Mat4f();
		for (int i = 0; i < 16; i++) {
			transff.m[i] = transf.m[i];
		}

		int meshId = p.part.drawMeshId;

		basicShader.setUniform("modelMatrix", transff);
		meshes[meshId]->render();
	}
	
	basicShader.setUniform("modelMatrix", Mat4f());
	// boxMesh->render();	
	// transMesh->render();

	vectorShader.bind();
	vectorShader.setUniform("projectionMatrix", projectionMatrix);
	vectorShader.setUniform("viewMatrix", viewMatrix);
	vectorShader.setUniform("viewPosition", Vec3f(camera.position.x, camera.position.y, camera.position.z));
	vectorMesh->render();

 	originShader.bind();
	originShader.setUniform("projectionMatrix", projectionMatrix);
	originShader.setUniform("viewMatrix", viewMatrix);
	originShader.setUniform("viewPosition", Vec3f(camera.position.x, camera.position.y, camera.position.z));
	originMesh->render();

	glfwSwapBuffers(this->window);
	glfwPollEvents();
}

void Screen::close() {
	basicShader.close();
	vectorShader.close();

	terminateGL();
}

bool Screen::shouldClose() {
	return glfwWindowShouldClose(window) != 0;
}

int Screen::addMeshShape(Shape s) {
	int size = meshes.size();
	meshes.push_back(new IndexedMesh(s));
	return size;
}
