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
PostProcessShader postProcessShader;
SkyboxShader skyboxShader;

Material material = Material (
	Vec3f(1, 1, 1),
	Vec3f(1, 1, 1),
	Vec3f(1, 1, 1),
	1
);

const int lightCount = 4;
Attenuation attenuation = { 0, 0, 0.2 };
Light lights[lightCount] = {
	Light(Vec3f(5, 0, 0), Vec3f(1, 0, 0), 4, attenuation),
	Light(Vec3f(0, 5, 0), Vec3f(0, 1, 0), 4, attenuation),
	Light(Vec3f(0, 0, 5), Vec3f(0, 0, 1), 4, attenuation),
	Light(Vec3f(0, 0, 0), Vec3f(1, 1, 1), 1, attenuation)
};

CubeMap* skybox = nullptr;

void Screen::init() {
	//Log::setLogLevel(Log::Level::NONE);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	screenSize = Vec2(width, height);

	ShaderSource basicShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(BASIC_SHADER)), "basic.shader");
	ShaderSource basicNormalShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(BASICNORMAL_SHADER)), "basicnormal.shader");
	ShaderSource vectorShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(VECTOR_SHADER)), "vector.shader");
	ShaderSource originShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(ORIGIN_SHADER)), "origin.shader");
	ShaderSource quadShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(QUAD_SHADER)), "quad.shader");
	ShaderSource postProcessShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(POSTPROCESS_SHADER)), "postProcess.shader");
	ShaderSource skyboxShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(SKYBOX_SHADER)), "skybox.shader");

	basicShader = * new BasicShader(basicShaderSource);
	basicNormalShader = * new BasicNormalShader(basicNormalShaderSource);
	vectorShader = * new VectorShader(vectorShaderSource);
	originShader = * new OriginShader(originShaderSource);
	quadShader = * new QuadShader(quadShaderSource);
	postProcessShader = * new PostProcessShader(postProcessShaderSource);
	skyboxShader = * new SkyboxShader(skyboxShaderSource);

	camera.setPosition(Vec3(1, 1, -2));
	camera.setRotation(Vec3(0, 3.1415, 0.0));

	basicShader.createLightArray(lightCount);

	handler = new StandardInputHandler(window, *this);

	quad = new Quad();
	modelFrameBuffer = new FrameBuffer(width, height);
	screenFrameBuffer = new FrameBuffer(width, height);
	quadShader.update(*modelFrameBuffer->texture);
	postProcessShader.update(*modelFrameBuffer->texture);

	skybox = new CubeMap("../res/skybox/right.jpg", "../res/skybox/left.jpg", "../res/skybox/top.jpg", "../res/skybox/bottom.jpg", "../res/skybox/front.jpg", "../res/skybox/back.jpg");

	box = new BoundingBox{-1, -1, -1, 1, 1, 1};
	Shape shape = box->toShape(new Vec3[8]);// .rotated(fromEulerAngles(0.5, 0.1, 0.2), new Vec3[8]);
	boxMesh = new IndexedMesh(shape);
	Shape trans = shape.translated(Vec3(1.0, -1.0, 1.0), new Vec3[8]);
	transMesh = new IndexedMesh(trans);
	double originVertices[3] = { 0, 0, 5 };
	originMesh = new ArrayMesh(originVertices, 1, 3, RenderMode::POINTS);
	double * vecs = new double[128 * 7];
	vecs[0] = 0.2;
	vecs[1] = 0.3;
	vecs[2] = 0.7;
	vecs[3] = 0.8;
	vecs[4] = 0.6;
	vecs[5] = 0.7;
	vecs[6] = 0.5;
	vectorMesh = new VectorMesh(vecs, 128);

	eventHandler.setPhysicalRayIntersectCallback([] (Screen& screen, Physical* physical, Vec3 point) {
		screen.intersectedPhysical = physical;
		screen.intersectedPoint = point;
	});

	eventHandler.setPhysicalClickCallback([] (Screen& screen, Physical* physical, Vec3 point) {
		screen.selectedPhysical = physical;
		screen.selectedPoint = point;
	});

	eventHandler.setWindowResizeCallback([] (Screen& screen, unsigned int width, unsigned int height) {
		screen.modelFrameBuffer->texture->resize(width, height);
		screen.screenFrameBuffer->texture->resize(width, height);
		screen.modelFrameBuffer->renderBuffer->resize(width, height);
		screen.screenFrameBuffer->renderBuffer->resize(width, height);
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

	
	static long long t = 0;
	t++;
	float d = 0.5 + 0.5 * sin(t * 0.02);

	lights[0].color = Vec3f(d, 0.3, 1-d);
	lights[1].color = Vec3f(1-d, 0.3, 1 - d);
	lights[2].color = Vec3f(0.2, 0.3*d, 1 - d);
	lights[3].color = Vec3f(1-d, 1-d, d);


	// Matrix calculations
	projectionMatrix = Mat4f().perspective(1.0, screenSize.x / screenSize.y, 0.01, 100000.0);
	orthoMatrix = Mat4f().ortho(-1, 1, -screenSize.x / screenSize.y, screenSize.x / screenSize.y, 0.1, 100);
	rotatedViewMatrix = camera.cframe.asMat4f().getRotation();
	viewMatrix = rotatedViewMatrix.translate(-camera.cframe.position.x, -camera.cframe.position.y, -camera.cframe.position.z);
	viewPosition = Vec3f(camera.cframe.position.x, camera.cframe.position.y, camera.cframe.position.z);

	world->lock.lock();
	updateIntersectedPhysical(*this, world->physicals, handler->curPos, screenSize, viewMatrix, projectionMatrix);
	world->lock.unlock();
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

void Screen::renderSkybox() {
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	skyboxShader.update(*skybox);
	skyboxShader.update(viewMatrix, projectionMatrix);
	skybox->bind();
	boxMesh->render();
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
}

void Screen::renderPhysicals() {
	// Bind basic uniforms
	basicShader.updateLight(lights, lightCount);
	basicShader.update(viewMatrix, projectionMatrix, viewPosition);
	basicShader.updateMaterial(material);

	std::lock_guard<std::mutex> lg(world->lock);
	// Render world objects
	for (Physical& physical : world->physicals) {
		int meshId = physical.part.drawMeshId;

		// Picker code
		if (&physical == selectedPhysical)
			material.ambient = Vec3f(0.5, 0.6, 0.3);
		else if (&physical == intersectedPhysical)
			material.ambient = Vec3f(0.5, 0.5, 0.5);
		else
			material.ambient = Vec3f(0.3, 0.4, 0.2);

		basicShader.updateMaterial(material);

		// Render each physical
		Mat4f transformation = physical.part.cframe.asMat4f();
		basicShader.updateModel(transformation);

		meshes[meshId]->render();
	}
}

void Screen::refresh() {

	// Render physicals to modelFrameBuffer
	modelFrameBuffer->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderSkybox();

	glEnable(GL_DEPTH_TEST);

	// Initialize vector log buffer
	AddableBuffer<AppDebug::ColoredVec> vecLog = AppDebug::getVecBuffer();

	renderPhysicals();

	// Postprocess to screenFrameBuffer
	screenFrameBuffer->bind();
	glDisable(GL_DEPTH_TEST);
	postProcessShader.bind();
	modelFrameBuffer->texture->bind();
	quad->render();

	// Render vectors with old depth buffer
	glEnable(GL_DEPTH_TEST);
	screenFrameBuffer->attach(modelFrameBuffer->renderBuffer);

	// Update vector mesh
	updateVecMesh(vecLog.data, vecLog.index);

	// Render lights
	for (Light light : lights) {
		Mat4f transformation = Mat4f().translate(light.position).scale(0.1);
		basicShader.updateMaterial(Material(light.color, Vec3f(), Vec3f(), 10));
		basicShader.updateModel(transformation);
		boxMesh->render();
	}

	// Render vector mesh
	vectorShader.update(viewMatrix, projectionMatrix, viewPosition);
	vectorMesh->render();

	// Render origin mesh
	originShader.update(viewMatrix, rotatedViewMatrix, projectionMatrix, orthoMatrix, viewPosition);
	originMesh->render();

	// Render screenFrameBuffer texture to the screen
	screenFrameBuffer->unbind();
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	quadShader.bind();
	screenFrameBuffer->texture->bind();
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