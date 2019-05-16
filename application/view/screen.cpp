#include "screen.h"

#include "shader.h"
#include "indexedMesh.h"
#include "arrayMesh.h"
#include "vectorMesh.h"
#include "picker.h"
#include "material.h"
#include "quad.h"
#include "shaderProgram.h"
#include "font.h"
#include "visualDebug.h"
#include "loader.h"

#include "gui\panel.h"
#include "gui\frame.h"
#include "gui\label.h"
#include "gui\slider.h"
#include "gui\checkBox.h"
#include "gui\gui.h"

#include "../debug.h"
#include "../standardInputHandler.h"
#include "../resourceManager.h"
#include "../objectLibrary.h"

#include "../util/log.h"

#include "../engine/math/vec2.h"
#include "../engine/math/mat4.h"
#include "../engine/math/mathUtil.h"
#include "../engine/geometry/shape.h"
#include "../engine/geometry/shape.h"
#include "../engine/physicsProfiler.h"
#include "../engine/geometry/boundingBox.h"
#include "../engine/sharedLockGuard.h"
#include "../engine/debug.h"

#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include <map>


bool initGLFW() {
	/* Initialize GLFW */
	if (!glfwInit()) {
		Log::error("GLFW failed to initialize");
		return false;
	}

	Log::info("GLFW initialized");
	return true;
}

bool initGLEW() {
	// Init GLEW after creating a valid rendering context
	if (glewInit() != GLEW_OK) {
		glfwTerminate();

		Log::error("GLEW Failed to initialize!");

		return false;
	}

	Log::info("GLEW initialized");
	return true;
}

void terminateGL() {
	Log::info("Closing GLFW");
	glfwTerminate();
	Log::info("Closed GLFW");
}

Screen::Screen() {};

Screen::Screen(int width, int height, World<ExtendedPart>* world) {
	setWorld(world);

	// Create a windowed mode window and its OpenGL context 
	this->window = glfwCreateWindow(width, height, "Physics3D", NULL, NULL);
	
	if (!this->window) {
		glfwTerminate();
		exit(-1);
	}

	// Make the window's context current 
	glfwMakeContextCurrent(this->window);

	Log::info("OpenGL vendor: (%s)", glGetString(GL_VENDOR));
	Log::info("OpenGL renderer: (%s)", glGetString(GL_RENDERER));
	Log::info("OpenGL version: (%s)", glGetString(GL_VERSION));
	Log::info("OpenGL shader version: (%s)", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

// Font
Font* font = nullptr;

// Handler
StandardInputHandler* handler = nullptr;

// Textures
Texture* floorTexture = nullptr;
Texture* floorNormal = nullptr;

//Skybox
IndexedMesh* sphere = nullptr;
BoundingBox* skybox = nullptr;
CubeMap* skyboxTexture = nullptr;
IndexedMesh* skyboxMesh = nullptr;

// Render uniforms
Mat4f projectionMatrix;
Mat4f orthoMatrix;
Mat4f rotatedViewMatrix;
Mat4f viewMatrix;
Vec3f viewPosition;

// Shaders
BasicShader basicShader;
BasicNormalShader basicNormalShader;
VectorShader vectorShader;
OriginShader originShader;
FontShader fontShader;
QuadShader quadShader;
PostProcessShader postProcessShader;
SkyboxShader skyboxShader;

// Light uniforms
const int lightCount = 4;
Vec3f sunDirection;
Attenuation attenuation = { 0, 0, 0.5 };
Light lights[lightCount] = {
	Light(Vec3f(5, 0, 0), Vec3f(1, 1, 1), 2, attenuation),
	Light(Vec3f(0, 5, 0), Vec3f(1, 1, 1), 2, attenuation),
	Light(Vec3f(0, 0, 5), Vec3f(1, 1, 1), 2, attenuation),
	Light(Vec3f(0, 0, 0), Vec3f(1, 1, 1), 2, attenuation)
};

// Render meshes
VectorMesh* vectorMesh = nullptr;
ArrayMesh* originMesh = nullptr;
Quad* quad = nullptr;

// GUI
Frame* propertiesFrame = nullptr;
Label* partNameLabel = nullptr;
Label* partPositionLabel = nullptr;
Label* partMeshIDLabel = nullptr;
Slider* partAmbientSlider = nullptr;
Slider* testSlider = nullptr;
CheckBox* renderModeCheckBox = nullptr;

Panel* mouseVertical = nullptr;
Panel* mouseHorizontal = nullptr;

void Screen::init() {
	// Log init
	Log::setLogLevel(Log::Level::INFO);


	// Properties init
	properties = PropertiesParser::read("../res/.properties");


	// Render mode init
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);


	// InputHandler init
	handler = new StandardInputHandler(window, *this);


	// Screen size init
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	handler->framebufferResize(width, height);


	// Shader source init
	ShaderSource basicShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(BASIC_SHADER)), "basic.shader");
	ShaderSource basicNormalShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(BASICNORMAL_SHADER)), "basicnormal.shader");
	ShaderSource vectorShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(VECTOR_SHADER)), "vector.shader");
	ShaderSource fontShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(FONT_SHADER)), "font.shader");
	ShaderSource originShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(ORIGIN_SHADER)), "origin.shader");
	ShaderSource quadShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(QUAD_SHADER)), "quad.shader");
	ShaderSource postProcessShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(POSTPROCESS_SHADER)), "postProcess.shader");
	ShaderSource skyboxShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(SKYBOX_SHADER)), "skybox.shader");


	// Shader init
	basicShader = * new BasicShader(basicShaderSource);
	basicNormalShader = * new BasicNormalShader(basicNormalShaderSource);
	vectorShader = * new VectorShader(vectorShaderSource);
	fontShader = * new FontShader(fontShaderSource);
	originShader = * new OriginShader(originShaderSource);
	quadShader = * new QuadShader(quadShaderSource);
	postProcessShader = * new PostProcessShader(postProcessShaderSource);
	skyboxShader = * new SkyboxShader(skyboxShaderSource);
	basicShader.createLightArray(lightCount);


	// Texture init
	/*floorTexture = load("../res/textures/floor/floor.jpg");
	//floorNormal = load("../res/textures/metal/metal_normal.jpg");
	floorNormal = load("../res/textures/floor/floor_normal.jpg");
	if(floorTexture != nullptr)
		defaultMaterial.setTexture(floorTexture);
	if(floorNormal != nullptr)
		defaultMaterial.setNormalMap(floorNormal);*/


	// Skybox init
	sphere = new IndexedMesh(loadObj((std::istream&) std::istringstream(getResourceAsString(SPHERE_MODEL))));
	skybox = new BoundingBox{ -1, -1, -1, 1, 1, 1 };
	skyboxMesh = new IndexedMesh(skybox->toShape(new Vec3[8]));
	skyboxTexture = new CubeMap("../res/skybox/right.jpg", "../res/skybox/left.jpg", "../res/skybox/top.jpg", "../res/skybox/bottom.jpg", "../res/skybox/front.jpg", "../res/skybox/back.jpg");


	// Camera init
	camera.setPosition(Vec3(1, 1, -2));
	camera.setRotation(Vec3(0, 3.1415, 0.0));


	// Framebuffer init
	quad = new Quad();
	modelFrameBuffer = new FrameBuffer(width, height);
	screenFrameBuffer = new FrameBuffer(width, height);


	// Font init
	font = new Font(fontShader, "../res/fonts/droid.ttf");


	// GUI init
	GUI::init(this, &quadShader, font);
	propertiesFrame = new Frame(0.7, 0.7, "Properties");
	partNameLabel = new Label("", 0, 0);
	partPositionLabel = new Label("", 0, 0);
	partMeshIDLabel = new Label("", 0, 0);
	partAmbientSlider = new Slider(0, 0, 0, 0.999999, 0.5);
	testSlider = new Slider(0, 0, 500, 3000, 600);

	partAmbientSlider->action = [] (Slider* s) {
		if (GUI::screen->selectedPart) {

			Log::debug("%f, %s", s->value, str(GUI::COLOR::hsvToRgb(Vec3(s->value, 1, 1))).c_str());

			GUI::screen->selectedPart->material.ambient = GUI::COLOR::hsvToRgb(Vec3(s->value, 1, 1));
		}
	};
	renderModeCheckBox = new CheckBox("Properties", 0, 0, true);
	renderModeCheckBox->action = [] (CheckBox* c) {
		if (GUI::screen->selectedPart) {
			if (GUI::screen->selectedPart->renderMode == GL_FILL) {
				GUI::screen->selectedPart->renderMode = GL_LINE;
			} else {
				GUI::screen->selectedPart->renderMode = GL_FILL;
			}
		}
	};
	propertiesFrame->add(partNameLabel, Align::FILL);
	propertiesFrame->add(partPositionLabel, Align::FILL);
	propertiesFrame->add(partMeshIDLabel, Align::FILL);
	propertiesFrame->add(renderModeCheckBox, Align::FILL);
	propertiesFrame->add(partAmbientSlider, Align::FILL);
	propertiesFrame->add(testSlider, Align::FILL);
	GUI::add(propertiesFrame);


	// Mouse init
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	mouseVertical = new Panel(0, 0, 0.04, 0.007);
	mouseHorizontal = new Panel(0, 0, 0.007, 0.04);
	mouseVertical->backgroundColor = Vec4(1);
	mouseHorizontal->backgroundColor = Vec4(1);
	

	// Origin init
	double originVertices[3] = { 0, 0, 5 };
	originMesh = new ArrayMesh(originVertices, 1, 3, RenderMode::POINTS);


	// Vector init
	vectorMesh = new VectorMesh(new double[128 * 7], 128);


	// Eventhandler init
	eventHandler.setPartRayIntersectCallback([] (Screen& screen, ExtendedPart* part, Vec3 point) {
		screen.intersectedPart = part;
		screen.intersectedPoint = point;
	});

	eventHandler.setPartClickCallback([] (Screen& screen, ExtendedPart* part, Vec3 point) {
		screen.selectedPart = part;
		screen.selectedPoint = point;
	});

	eventHandler.setWindowResizeCallback([] (Screen& screen, unsigned int width, unsigned int height) {
		screen.modelFrameBuffer->texture->resize(width, height);
		screen.screenFrameBuffer->texture->resize(width, height);
		screen.modelFrameBuffer->renderBuffer->resize(width, height);
		screen.screenFrameBuffer->renderBuffer->resize(width, height);
		screen.dimension = Vec2(width, height);
		screen.aspect = ((double) width) / ((double) height);
	});

	// Temp
	handler->framebufferResize(width, height);
}

void Screen::update() {

	// IO events
	if (handler->anyKey) {
		bool leftDragging = handler->leftDragging;
		if (handler->getKey(GLFW_KEY_W))  camera.move(*this, 0, 0, -1, leftDragging);
		if (handler->getKey(GLFW_KEY_S))  camera.move(*this, 0, 0, 1, leftDragging);	
		if (handler->getKey(GLFW_KEY_D))  camera.move(*this, 1, 0, 0, leftDragging);
		if (handler->getKey(GLFW_KEY_A))  camera.move(*this, -1, 0, 0, leftDragging);
		if (handler->getKey(GLFW_KEY_SPACE)) 
			if (camera.flying) camera.move(*this, 0, 1, 0, leftDragging);
		if (handler->getKey(GLFW_KEY_LEFT_SHIFT)) 
			if (camera.flying) camera.move(*this, 0, -1, 0, leftDragging);
		if (handler->getKey(GLFW_KEY_LEFT))  camera.rotate(*this, 0, -1, 0, leftDragging);
		if (handler->getKey(GLFW_KEY_RIGHT)) camera.rotate(*this, 0, 1, 0, leftDragging);
		if (handler->getKey(GLFW_KEY_UP))    camera.rotate(*this, -1, 0, 0, leftDragging);
		if (handler->getKey(GLFW_KEY_DOWN))  camera.rotate(*this, 1, 0, 0, leftDragging);
		if (handler->getKey(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, GLFW_TRUE);
	}


	// Update camera
	camera.update();
	

	// Update lights
	/*static long long t = 0;
	float d = 0.5 + 0.5 * sin(t++ * 0.005);
	sunDirection = Vec3f(0, cos(t * 0.005) , sin(t * 0.005));
	lights[0].color = Vec3f(d, 0.3, 1-d);
	lights[1].color = Vec3f(1-d, 0.3, 1 - d);
	lights[2].color = Vec3f(0.2, 0.3*d, 1 - d);
	lights[3].color = Vec3f(1-d, 1-d, d);*/


	// Update render uniforms
	float size = 10;
	// projectionMatrix = ortho(-size*aspect, size*aspect, -size, size, -1000, 1000);
	projectionMatrix = perspective(1.0, aspect, 0.01, 1000000.0);
	orthoMatrix = ortho(-aspect, aspect, -1, 1, -1000, 1000);
	//orthoMatrix = ortho(0, aspect, 0, 1, -1000, 1000);
	rotatedViewMatrix = camera.cframe.asMat4f().getRotation();
	viewMatrix = rotatedViewMatrix.translate(-camera.cframe.position.x, -camera.cframe.position.y, -camera.cframe.position.z);
	viewPosition = Vec3f(camera.cframe.position.x, camera.cframe.position.y, camera.cframe.position.z);


	// Update picker
	updateIntersectedPhysical(*this, handler->cursorPosition, viewMatrix, projectionMatrix);


	// Update gui
	mouseVertical->position = GUI::map(handler->cursorPosition) + Vec2(-mouseVertical->dimension.x / 2, mouseVertical->dimension.y / 2);
	mouseHorizontal->position = GUI::map(handler->cursorPosition) + Vec2(-mouseHorizontal->dimension.x / 2, mouseHorizontal->dimension.y / 2);

	GUI::intersect(GUI::map(handler->cursorPosition));

	if (selectedPart) {
		partMeshIDLabel->text = "MeshID: " + std::to_string(selectedPart->drawMeshId);
		renderModeCheckBox->checked = selectedPart->renderMode == GL_FILL;
		partPositionLabel->text = "Position: " + str(selectedPart->cframe.position);
		partNameLabel->text = "Name: " + selectedPart->name;
		Vec3 color = GUI::COLOR::rgbToHsv(selectedPart->material.ambient);
		partAmbientSlider->handleColor = Vec4(selectedPart->material.ambient.x, selectedPart->material.ambient.y, selectedPart->material.ambient.z, 1);
		partAmbientSlider->value = partAmbientSlider->min + (partAmbientSlider->max - partAmbientSlider->min) * color.x;
	} else {
		partMeshIDLabel->text = "MeshID: -";
		renderModeCheckBox->checked = false;
		partPositionLabel->text = "Position: -";
		partNameLabel->text = "Name: -";
		partAmbientSlider->handleColor = GUI::COLOR::BACK;
		partAmbientSlider->value = (partAmbientSlider->min + partAmbientSlider->max) / 2.0;
	}
}

void Screen::renderSkybox() {
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	skyboxShader.update(sunDirection);
	skyboxShader.update(viewMatrix, projectionMatrix);
	skyboxTexture->bind();
	sphere->render();
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
}

void Screen::renderPhysicals() {
	// Bind basic uniforms
	basicShader.updateLight(lights, lightCount);

	SharedLockGuard lg(world->lock);
	
	// Render world objects
	for (ExtendedPart& part : *world) {
		int meshId = part.drawMeshId;

		Material material = part.material;

		// Picker code
		if(&part == selectedPart)
			material.ambient = part.material.ambient + Vec3(0.1);
		else if (&part == intersectedPart)
			material.ambient = part.material.ambient + Vec3(-0.1);
		else
			material.ambient = part.material.ambient;
		
		basicShader.updateMaterial(material);

		// Render each physical
		basicShader.updatePart(part);

		if(meshId == -1) continue;

		meshes[meshId]->render(part.renderMode);
	}
}

void Screen::refresh() {
	fieldIndex = 0;

	// Render skybox
	graphicsMeasure.mark(GraphicsProcess::SKYBOX);
	modelFrameBuffer->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderSkybox();

	glEnable(GL_DEPTH_TEST);


	// Initialize vector log buffer
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	AddableBuffer<AppDebug::ColoredVec> vecLog = AppDebug::getVecBuffer();


	// Render physicals
	graphicsMeasure.mark(GraphicsProcess::PHYSICALS);
	basicShader.update(viewMatrix, projectionMatrix, viewPosition);
	renderPhysicals();


	// Postprocess to screenFrameBuffer
	screenFrameBuffer->bind();
	glDisable(GL_DEPTH_TEST);
	postProcessShader.update(modelFrameBuffer->texture);
	quad->render();


	// Render vectors with old depth buffer
	glEnable(GL_DEPTH_TEST);
	screenFrameBuffer->attach(modelFrameBuffer->renderBuffer);
	
	/*for (ExtendedPart& part : *world) {
		if (part.hitbox.normals)
			for (int i = 0; i < part.hitbox.vertexCount; i++)
				vecLog.add(AppDebug::ColoredVec(part.cframe.localToGlobal(part.hitbox.vertices[i]), part.cframe.localToRelative(part.hitbox.normals.get()[i]), Debug::POSITION));
	}*/

	// Update vector mesh
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	updateVecMesh(vectorMesh, vecLog.data, vecLog.index);


	// Render lights
	graphicsMeasure.mark(GraphicsProcess::LIGHTING);
	for (Light light : lights) {
		Mat4f transformation = Mat4f().translate(light.position).scale(0.1);
		basicShader.updateMaterial(Material(light.color, Vec3f(), Vec3f(), 10));
		basicShader.updateModelMatrix(transformation);
		skyboxMesh->render();
	}
	

	// Render vector mesh
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	vectorShader.update(viewMatrix, projectionMatrix, viewPosition);
	vectorMesh->render();


	// Render origin mesh
	graphicsMeasure.mark(GraphicsProcess::ORIGIN);
	originShader.update(viewMatrix, rotatedViewMatrix, projectionMatrix, orthoMatrix, viewPosition);
	originMesh->render();


	// Render screenFrameBuffer texture to the screen
	graphicsMeasure.mark(GraphicsProcess::FINALIZE);
	screenFrameBuffer->unbind();
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);


	// Render postprocessed image to screen
	quadShader.update(Mat4f());
	quadShader.update(screenFrameBuffer->texture);
	quad->render();


	// Render GUI
	glDisable(GL_DEPTH_TEST);
	graphicsMeasure.mark(GraphicsProcess::OTHER);
	fontShader.update(orthoMatrix);
	GUI::render(orthoMatrix);
	mouseVertical->render();
	mouseHorizontal->render();


	// Pie rendering
	graphicsMeasure.mark(GraphicsProcess::PROFILER);
	size_t objCount = world->partCount;
	renderDebugField(dimension, font, "Position", str(camera.cframe.position), "");
	renderDebugField(dimension, font, "Objects", objCount, "");
	renderDebugField(dimension, font, "Intersections", getTheoreticalNumberOfIntersections(objCount), "");
	renderDebugField(dimension, font, "AVG Collide GJK Iterations", gjkCollideIterStats.avg(), "");
	renderDebugField(dimension, font, "AVG No Collide GJK Iterations", gjkNoCollideIterStats.avg(), "");
	renderDebugField(dimension, font, "TPS", physicsMeasure.getAvgTPS(), "");
	renderDebugField(dimension, font, "FPS", graphicsMeasure.getAvgTPS(), "");

	if (renderPies) {
		float leftSide = dimension.x / dimension.y;
		PieChart graphicsPie = toPieChart(graphicsMeasure, "Graphics", Vec2f(-leftSide + 1.5f, -0.7f), 0.2f);
		PieChart physicsPie = toPieChart(physicsMeasure, "Physics", Vec2f(-leftSide + 0.3f, -0.7f), 0.2f);
		PieChart intersectionPie = toPieChart(intersectionStatistics, "Intersection Statistics", Vec2f(-leftSide + 2.7f, -0.7f), 0.2f);
		physicsPie.renderText(*this, font);
		graphicsPie.renderText(*this, font);
		intersectionPie.renderText(*this, font);
		startPieRendering(*this);
		physicsPie.renderPie(*this);
		graphicsPie.renderPie(*this);
		intersectionPie.renderPie(*this);
		endPieRendering(*this);
	}


	// Render stuff
	glfwSwapBuffers(this->window);
	glfwPollEvents();
}

void Screen::close() {
	basicShader.close();
	basicNormalShader.close();
	vectorShader.close();
	fontShader.close();
	originShader.close();
	skyboxShader.close();
	quadShader.close();
	postProcessShader.close();

	PropertiesParser::write("../res/.properties", properties);

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