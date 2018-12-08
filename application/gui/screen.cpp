#include "screen.h"
#include "../../util/log.h"
#include "../engine/math/vec2.h"
#include "../engine/math/mat4.h"
#include "shader.h"
#include "indexedMesh.h"
#include "vectorMesh.h"
#include "../engine/geometry/shape.h"
#include "camera.h"
#include "../standardInputHandler.h"
#include "../engine/geometry/shape.h"
#include "../engine/geometry/boundingBox.h"
#include "picker.h"
#include "../../engine/math/mathUtil.h"

#include "../resourceManager.h"

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

const unsigned int vertexCount = 6;
const double vertices[vertexCount * 7]{
	0, 0, 0,	  1,  0,  0,	0,
	0, 0, 0,	 -1,  0,  0,	0.1,
	0, 0, 0,	  0,  1,  0,	0.2,
	0, 0, 0,	  0, -1,  0,	0.3,
	0, 0, 0,	  0,  0,  1,	0.4,
	0, 0, 0,	  0,  0, -1,	1
};

void generateShape(double* buffer, int mi, int mj) {
	double twopi = 6.28318530718;
	int ind = 0;
	for (int i = 0; i < mi; i++) {
		for (int j = 0; j < mj; j++) {
			double ri = twopi / 2 / mi * i;
			double rj = twopi / mj * j;

			buffer[ind * 7 + 0] = 0;
			buffer[ind * 7 + 1] = 0;
			buffer[ind * 7 + 2] = 0;

			buffer[ind * 7 + 3] = sin(ri) * cos(rj);
			buffer[ind * 7 + 4] = sin(ri) * sin(rj);
			buffer[ind * 7 + 5] = cos(ri);
			
			buffer[ind * 7 + 6] = 1.0 / mi / mj * ind;

			ind++;
		}
	}
}

IndexedMesh* boxMesh = nullptr;
VectorMesh* vectorMesh = nullptr;

IndexedMesh* transMesh = nullptr;

Shader basicShader;
Shader vectorShader;
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

	basicShader = Shader(basicShaderSource);
	basicShader.createUniform("modelMatrix");
	basicShader.createUniform("viewMatrix");
	basicShader.createUniform("color");
	basicShader.createUniform("projectionMatrix");
	basicShader.createUniform("viewPosition");

	vectorShader = Shader(vectorShaderSource);
	vectorShader.createUniform("viewMatrix");
	vectorShader.createUniform("projectionMatrix");
	basicShader.createUniform("viewPosition");

	camera.setPosition(-1, 1, 4);

	handler = new StandardInputHandler(window, this, &camera);

	box = new BoundingBox{-0.5, -0.5, -0.5, 0.5, 0.5, 0.5};
	Shape shape = box->toShape(new Vec3[8]);// .rotated(fromEulerAngles(0.5, 0.1, 0.2), new Vec3[8]);

	boxMesh = new IndexedMesh(shape);

	Shape trans = shape.translated(Vec3(1.0, -1.0, 1.0), new Vec3[8]);
	transMesh = new IndexedMesh(trans);
	
	const int mi = 50;
	const int mj = 50;
	double* vert = new double[mi * mj * 7];
	
	generateShape(vert, mi, mj);

	vectorMesh = new VectorMesh(vertices, vertexCount);
	//vectorMesh = new VectorMesh(vert, mi * mj);
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

bool intersect(Vec3 orig, Vec3 dir, Vec3 min, Vec3 max) {
	float tmin = (min.x - orig.x) / dir.x;
	float tmax = (max.x - orig.x) / dir.x;

	if (tmin > tmax) {
		double temp = tmin;
		tmin = tmax;
		tmax = temp;
	};

	float tymin = (min.y - orig.y) / dir.y;
	float tymax = (max.y - orig.y) / dir.y;

	if (tymin > tymax) {
		double temp = tymin;
		tymin = tymax;
		tymax = temp;
	}

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (min.z - orig.z) / dir.z;
	float tzmax = (max.z - orig.z) / dir.z;

	if (tzmin > tzmax) {
		double temp = tzmin;
		tzmin = tzmax;
		tzmax = temp;
	}

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}


void Screen::refresh() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	transMesh->render();

	vectorShader.bind();
	vectorShader.setUniform("projectionMatrix", projectionMatrix);
	vectorShader.setUniform("viewMatrix", viewMatrix);
	vectorShader.setUniform("viewPosition", Vec3f(camera.position.x, camera.position.y, camera.position.z));
	vectorMesh->render();

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
