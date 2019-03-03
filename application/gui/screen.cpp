#include "screen.h"

#include "shader.h"
#include "indexedMesh.h"
#include "arrayMesh.h"
#include "vectorMesh.h"
#include "picker.h"
#include "material.h"
#include "quad.h"

#include "shaderProgram.h"

#include "../debug.h"
#include "../standardInputHandler.h"
#include "../resourceManager.h"
#include "../objectLibrary.h"

#include "../../util/log.h"

#include "../engine/math/vec2.h"
#include "../engine/math/mat4.h"
#include "../engine/math/mathUtil.h"
#include "../engine/geometry/shape.h"
#include "../engine/geometry/shape.h"
#include "../engine/geometry/boundingBox.h"
#include "../engine/debug.h"

#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include <map>

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

Screen::Screen(int width, int height, World* world) {
	setWorld(world);

	/* Create a windowed mode window and its OpenGL context */
	this->window = glfwCreateWindow(width, height, "Physics3D", NULL, NULL);
	if (!this->window) {
		glfwTerminate();
		exit(-1);
	}

	/* Make the window's context current */
	makeCurrent();

	Log::info("OpenGL vendor: (%s)", glGetString(GL_VENDOR));
	Log::info("OpenGL renderer: (%s)", glGetString(GL_RENDERER));
	Log::info("OpenGL version: (%s)", glGetString(GL_VERSION));
	Log::info("OpenGL shader version: (%s)", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

IndexedMesh* boxMesh = nullptr;
VectorMesh* vectorMesh = nullptr;
ArrayMesh* originMesh = nullptr;
IndexedMesh* transMesh = nullptr;

Quad* quad = nullptr;

BoundingBox* box = nullptr;
StandardInputHandler* handler = nullptr;

using namespace Debug;
std::map<VecType, bool> debug_enabled{ {INFO, true}, {VELOCITY, true}, {FORCE, true}, {POSITION, true}, {MOMENT, true}, {IMPULSE, true}, {ANGULAR_VELOCITY , true} };
std::map<VecType, double> vecColors{ {INFO, 0.15}, {VELOCITY, 0.3}, {FORCE, 0.0}, {POSITION, 0.5}, {MOMENT, 0.1}, {IMPULSE, 0.7}, {ANGULAR_VELOCITY , 0.75} };

/*
if (!debug_enabled[type]) 
	return;
double color;
switch (type) {
	case INFO: color = 0.15; break;
	case FORCE: color = 0.0; break;
	case MOMENT: color = 0.1; break;
	case IMPULSE: color = 0.7; break;
	case POSITION: color = 0.5; break;
	case VELOCITY: color = 0.3; break;
	case ANGULAR_VELOCITY: color = 0.75; break;
}
*/

BasicShader basicShader;
BasicNormalShader basicNormalShader;
VectorShader vectorShader;
OriginShader originShader;
QuadShader quadShader;

Material material = Material (
	Vec4f(0.3f, 0.4f, 0.2f, 1.0f),
	Vec4f(0.3f, 0.2f, 0.6f, 1.0f),
	Vec4f(0.2f, 0.1f, 0.9f, 1.0f),
	0.5f
);

void Screen::init() {
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	screenSize = Vec2(width, height);

	ShaderSource basicShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(BASIC_SHADER)), "basic.shader");
	ShaderSource basicNormalShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(BASICNORMAL_SHADER)), "basicnormal.shader");
	ShaderSource vectorShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(VECTOR_SHADER)), "vector.shader");
	ShaderSource originShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(ORIGIN_SHADER)), "origin.shader");;
	ShaderSource quadShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(QUAD_SHADER)), "quad.shader");;

	basicShader = * new BasicShader(basicShaderSource);
	basicNormalShader = * new BasicNormalShader(basicNormalShaderSource);
	vectorShader = * new VectorShader(vectorShaderSource);
	originShader = * new OriginShader(originShaderSource);
	quadShader = * new QuadShader(quadShaderSource);

	camera.setPosition(Vec3(1, 1, -2));
	camera.setRotation(Vec3(0, 3.1415, 0.0));

	handler = new StandardInputHandler(window, *this);

	quad = new Quad();
	frameBuffer = new FrameBuffer(width, height);
	quadShader.update(*frameBuffer->texture);

	box = new BoundingBox{-0.5, -0.5, -0.5, 0.5, 0.5, 0.5};
	Shape shape = box->toShape(new Vec3[8]);// .rotated(fromEulerAngles(0.5, 0.1, 0.2), new Vec3[8]);
	boxMesh = new IndexedMesh(shape);
	Shape trans = shape.translated(Vec3(1.0, -1.0, 1.0), new Vec3[8]);
	transMesh = new IndexedMesh(trans);
	double originVertices[3] = { 0, 0, 5 };
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

	eventHandler.setPhysicalRayIntersectCallback([] (Screen& screen, Physical* physical, Vec3 point) {
		screen.intersectedPhysical = physical;
		screen.intersectedPoint = point;
	});

	eventHandler.setPhysicalClickCallback([] (Screen& screen, Physical* physical, Vec3 point) {
		screen.selectedPhysical = physical;
		screen.selectedPoint = point;
	});

	eventHandler.setWindowResizeCallback([] (Screen& screen, unsigned int width, unsigned int height) {
		screen.frameBuffer->texture->resize(width, height);
		screen.frameBuffer->renderBuffer->resize(width, height);
	});
}

void Screen::makeCurrent() {
	glfwMakeContextCurrent(this->window);
}

// To be moved elsewhere
Mat4f projectionMatrix;
Mat4f orthoMatrix;
Mat4f rotatedViewMatrix;
Mat4f viewMatrix;
Vec3f viewPosition;

void Screen::update() {

	// IO events
	static double speed = 0.02;
	if (handler->anyKey) {
		bool leftDragging = handler->leftDragging;
		if (handler->getKey(GLFW_KEY_1)) {
			speed = 0.02;
		}
		if (handler->getKey(GLFW_KEY_2)) {
			speed = 0.1;
		}
		if (handler->getKey(GLFW_KEY_W)) {
			camera.move(*this, 0, 0, -speed, leftDragging);
		}
		if (handler->getKey(GLFW_KEY_S)) {
			camera.move(*this, 0, 0, speed, leftDragging);
		}
		if (handler->getKey(GLFW_KEY_D)) {
			camera.move(*this, speed, 0, 0, leftDragging);
		}
		if (handler->getKey(GLFW_KEY_A)) {
			camera.move(*this, -speed, 0, 0, leftDragging);
		}
		if (handler->getKey(GLFW_KEY_SPACE)) {
			camera.move(*this, 0, speed, 0, leftDragging);
		}
		if (handler->getKey(GLFW_KEY_LEFT_SHIFT)) {
			camera.move(*this, 0, -speed, 0, leftDragging);
		}
		if (handler->getKey(GLFW_KEY_LEFT)) {
			camera.rotate(*this, 0, -speed, 0, leftDragging);
		}
		if (handler->getKey(GLFW_KEY_RIGHT)) {
			camera.rotate(*this, 0, speed, 0, leftDragging);
		}
		if (handler->getKey(GLFW_KEY_UP)) {
			camera.rotate(*this, -speed, 0, 0, leftDragging);
		}
		if (handler->getKey(GLFW_KEY_DOWN)) {
			camera.rotate(*this, speed, 0, 0, leftDragging);
		}
		if (handler->getKey(GLFW_KEY_ESCAPE)) {
			exit(0);
		}
	}

	// Matrix calculations
	projectionMatrix = Mat4f().perspective(1.0, screenSize.x / screenSize.y, 0.01, 100000.0);
	orthoMatrix = Mat4f().ortho(-1, 1, -screenSize.x / screenSize.y, screenSize.x / screenSize.y, 0.1, 100);
	rotatedViewMatrix = camera.cframe.asMat4f().getRotation();
	viewMatrix = rotatedViewMatrix.translate(-camera.cframe.position.x, -camera.cframe.position.y, -camera.cframe.position.z);
	viewPosition = Vec3f(camera.cframe.position.x, camera.cframe.position.y, camera.cframe.position.z);

	updateIntersectedPhysical(*this, world->physicals, handler->curPos, screenSize, viewMatrix, projectionMatrix);
}

AddableBuffer<double> visibleVecs(700);

void updateVecMesh(AppDebug::ColoredVec* data, size_t size) {
	visibleVecs.clear();

	for(size_t i = 0; i < size; i++) {
		const AppDebug::ColoredVec& v = data[i];
		if(debug_enabled[v.type]) {
			visibleVecs.add(v.origin.x);
			visibleVecs.add(v.origin.y);
			visibleVecs.add(v.origin.z);
			visibleVecs.add(v.vec.x);
			visibleVecs.add(v.vec.y);
			visibleVecs.add(v.vec.z);
			visibleVecs.add(vecColors[v.type]);
		}
	}

	vectorMesh->update(visibleVecs.data, visibleVecs.index / 7);
}

void Screen::refresh() {
	// Render to frameBuffer
	frameBuffer->bind();

	// Clear GL buffer bits
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Initialize vector log buffer
	AddableBuffer<AppDebug::ColoredVec> vecLog = AppDebug::getVecBuffer();

	// Bind basic uniforms
	basicShader.update(viewMatrix, projectionMatrix, viewPosition);
	basicShader.updateMaterial(material);
	
	// Render world objects
	for (Physical& physical : world->physicals) {
		int meshId = physical.part.drawMeshId;

		// Picker code
		if (&physical == selectedPhysical)
			basicShader.updateColor(Vec3f(0.6, 0.8, 0.4));
		else if (&physical == intersectedPhysical)
			basicShader.updateColor(Vec3f(0.5, 0.5, 0.5));
		else
			basicShader.updateColor(Vec3f(0.3, 0.4, 0.2));

		// Render each physical
		Mat4f transformation = physical.part.cframe.asMat4f();
		basicShader.updateModel(transformation);
		meshes[meshId]->render();    
		
		//for (int i = 0; i < physical.part.hitbox.vertexCount; i++)
		//	vecLog.add(AppDebug::ColoredVec(physical.part.cframe.localToGlobal(physical.part.hitbox.vertices[i]), physical.part.cframe.rotation * physical.part.hitbox.normals[i], Debug::POSITION));
	}
	
	// Update vector mesh
	updateVecMesh(vecLog.data, vecLog.index);

	// Render vector mesh
	vectorShader.update(viewMatrix, projectionMatrix, viewPosition);
	vectorMesh->render();

	// Render origin mesh
	originShader.update(viewMatrix, rotatedViewMatrix, projectionMatrix, orthoMatrix, viewPosition);
	originMesh->render();

	// Render to screen
	frameBuffer->unbind();

	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	// Use frameBuffer texture
	frameBuffer->texture->bind();
	quadShader.bind();
	quad->render();

	glfwSwapBuffers(this->window);
	glfwPollEvents();
}

void Screen::close() {
	basicShader.close();
	vectorShader.close();
	originShader.close();

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

void Screen::toggleDebugVecType(Debug::VecType t) {
	debug_enabled[t] = !debug_enabled[t];
}