#include "screen.h"

#include "../import.h"
#include "shader.h"
#include "indexedMesh.h"
#include "arrayMesh.h"
#include "vectorMesh.h"
#include "pointMesh.h"
#include "picker.h"
#include "material.h"
#include "shaderProgram.h"
#include "font.h"
#include "visualDebug.h"
#include "loader.h"

#include "gui\panel.h"
#include "gui\frame.h"
#include "gui\label.h"
#include "gui\image.h"
#include "gui\slider.h"
#include "gui\checkBox.h"
#include "gui\colorPicker.h"
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
	// Initialize GLFW
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

Screen::Screen(int width, int height, MagnetWorld* world) {
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


// Skybox
IndexedMesh* sphere = nullptr;
BoundingBox* skybox = nullptr;
CubeMap* skyboxTexture = nullptr;
IndexedMesh* skyboxMesh = nullptr;


// Render uniforms
Mat4f orthoMatrix;


// Shaders
BasicShader basicShader;
DepthShader depthShader;
VectorShader vectorShader;
OriginShader originShader;
FontShader fontShader;
QuadShader quadShader;
PostProcessShader postProcessShader;
SkyboxShader skyboxShader;
PointShader pointShader;
TestShader testShader;
BlurShader blurShader;
ColorWheelShader colorWheelShader;

BarChartClassInformation iterChartClasses[]{ {"GJK Collide", Vec3f(0.2f,0.2f,1)},{"GJK No Collide", Vec3f(1.0f, 0.5f, 0.0f)},{"EPA", Vec3f(1.0f, 1.0f, 0.0f)} };
BarChart iterationChart("Iteration Statistics", "", GJKCollidesIterationStatistics.labels, iterChartClasses, Vec2f(-1 + 0.1f, -0.3), Vec2f(0.8, 0.6), 3, 17);


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


// Shadow
DepthFrameBuffer* depthBuffer;


// Render meshes
VectorMesh* vectorMesh = nullptr;
PointMesh* pointMesh = nullptr;
ArrayMesh* originMesh = nullptr;


// Properties GUI
Frame* propertiesFrame = nullptr;
Label* partNameLabel = nullptr;
Label* partPositionLabel = nullptr;
Label* partMeshIDLabel = nullptr;
Label* partVelocity = nullptr;
Label* partAngularVelocity = nullptr;
Label* partKineticEnergy = nullptr;
Label* partPotentialEnergy = nullptr;
Label* partEnergy = nullptr;
Button* colorButton;
CheckBox* renderModeCheckBox = nullptr;


// Mouse GUI
Panel* mouseVertical = nullptr;
Panel* mouseHorizontal = nullptr;


// Colorpicker GUI
Frame* colorPickerFrame = nullptr;
ColorPicker* colorPicker = nullptr;


// Debug GUI
Frame* debugFrame = nullptr;
Label* debugVectorLabel = nullptr;
CheckBox* debugInfoVectorCheckBox = nullptr;
CheckBox* debugPositionCheckBox = nullptr;
CheckBox* debugVelocityCheckBox = nullptr;
CheckBox* debugMomentCheckBox = nullptr;
CheckBox* debugForceCheckBox = nullptr;
CheckBox* debugAccelerationCheckBox = nullptr;
CheckBox* debugAngularImpulseCheckBox = nullptr;
CheckBox* debugImpulseCheckBox = nullptr;
CheckBox* debugAngularVelocityCheckBox = nullptr;
Label* debugPointLabel = nullptr;
CheckBox* debugInfoPointCheckBox = nullptr;
CheckBox* debugCenterOfMassCheckBox = nullptr;
CheckBox* debugIntersectionCheckBox = nullptr;
Label* debugRenderLabel = nullptr;
CheckBox* debugRenderPiesCheckBox = nullptr;
CheckBox* debugRenderSpheresCheckBox = nullptr;


// Test
IndexedMesh* mesh = nullptr;
Texture* texture = nullptr;

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
	ShaderSource depthShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(DEPTH_SHADER)), "depth.shader");
	ShaderSource vectorShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(VECTOR_SHADER)), "vector.shader");
	ShaderSource fontShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(FONT_SHADER)), "font.shader");
	ShaderSource originShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(ORIGIN_SHADER)), "origin.shader");
	ShaderSource quadShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(QUAD_SHADER)), "quad.shader");
	ShaderSource postProcessShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(POSTPROCESS_SHADER)), "postProcess.shader");
	ShaderSource skyboxShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(SKYBOX_SHADER)), "skybox.shader");
	ShaderSource pointShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(POINT_SHADER)), "point.shader");
	ShaderSource testShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(TEST_SHADER)), "test.shader");
	ShaderSource blurShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(BLUR_SHADER)), "blur.shader");
	ShaderSource colorWheelShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(COLORWHEEL_SHADER)), "colorwheel.shader");


	// Shader init
	basicShader = * new BasicShader(basicShaderSource);
	depthShader = * new DepthShader(depthShaderSource);
	vectorShader = * new VectorShader(vectorShaderSource);
	fontShader = * new FontShader(fontShaderSource);
	originShader = * new OriginShader(originShaderSource);
	quadShader = * new QuadShader(quadShaderSource);
	postProcessShader = * new PostProcessShader(postProcessShaderSource);
	skyboxShader = * new SkyboxShader(skyboxShaderSource);
	pointShader = * new PointShader(pointShaderSource);
	testShader = * new TestShader(testShaderSource);
	blurShader = * new BlurShader(blurShaderSource);
	colorWheelShader = * new ColorWheelShader(colorWheelShaderSource);
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
	VisualShape sphereShape(OBJImport::load((std::istream&) std::istringstream(getResourceAsString(SPHERE_MODEL))));
	sphere = new IndexedMesh(sphereShape);
	skybox = new BoundingBox{ -1, -1, -1, 1, 1, 1 };
	skyboxMesh = new IndexedMesh(VisualShape(skybox->toShape()));
	skyboxTexture = new CubeMap("../res/skybox/right.jpg", "../res/skybox/left.jpg", "../res/skybox/top.jpg", "../res/skybox/bottom.jpg", "../res/skybox/front.jpg", "../res/skybox/back.jpg");


	// Camera init
	camera.setPosition(Vec3(1, 1, -2));
	camera.setRotation(Vec3(0, 3.1415, 0.0));
	camera.update(1.0, 1, 0.01, 10000.0);


	// Framebuffer init
	quad = new Quad();
	modelFrameBuffer = new FrameBuffer(width, height);
	screenFrameBuffer = new FrameBuffer(width, height);
	blurFrameBuffer = new FrameBuffer(width, height);
	depthBuffer = new DepthFrameBuffer(1024, 1024);


	// Font init
	font = new Font(fontShader, "../res/fonts/droid.ttf");


	// GUI init
	GUI::init(this, &quadShader, &blurShader, font);


	// Properties GUI
	propertiesFrame = new Frame(0.7, 0.7, "Properties");
	partNameLabel = new Label("", 0, 0);
	partPositionLabel = new Label("", 0, 0);
	partMeshIDLabel = new Label("", 0, 0);
	colorButton = new Button(0, 0, GUI::sliderBarWidth, GUI::sliderHandleHeight, false);
	partVelocity = new Label("", 0, 0);
	partAngularVelocity = new Label("", 0, 0);
	partKineticEnergy = new Label("", 0, 0);
	partPotentialEnergy = new Label("", 0, 0);
	partEnergy = new Label("", 0, 0);
	renderModeCheckBox = new CheckBox("Wireframe", 0, 0, true);
	renderModeCheckBox->action = [](CheckBox* c) {
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
	propertiesFrame->add(colorButton, Align::FILL);
	propertiesFrame->add(partVelocity, Align::FILL);
	propertiesFrame->add(partAngularVelocity, Align::FILL);
	propertiesFrame->add(partKineticEnergy, Align::FILL);
	propertiesFrame->add(partPotentialEnergy, Align::FILL);
	propertiesFrame->add(partEnergy, Align::FILL);


	// Colorpicker GUI
	colorPickerFrame = new Frame(0, 0, "Color");
	colorPickerFrame->visible = false;
	colorPicker = new ColorPicker(0, 0, 0.5);
	colorPickerFrame->anchor = propertiesFrame;
	colorButton->action = [] (Button* c) {
		if (GUI::screen->selectedPart && !colorPickerFrame->visible) {
			colorPickerFrame->visible = true;
			colorPickerFrame->anchor = propertiesFrame;
		}
	};
	colorPicker->action = [] (ColorPicker* c) {
		if (GUI::screen->selectedPart) {
			GUI::screen->selectedPart->material.ambient = GUI::COLOR::hsvaToRgba(c->hsva);
		}
	};

	colorPickerFrame->add(colorPicker, Align::FILL);


	// Debug GUI
	debugFrame = new Frame(0.9, 0.9, "Debug");
	debugVectorLabel = new Label("Vectors", 0, 0);
	debugInfoVectorCheckBox = new CheckBox("Info", 0, 0, true);
	debugPositionCheckBox = new CheckBox("Position", 0, 0, true);
	debugVelocityCheckBox = new CheckBox("Velocity", 0, 0, true);
	debugAccelerationCheckBox = new CheckBox("Acceleration", 0, 0, true);
	debugForceCheckBox = new CheckBox("Force", 0, 0, true);
	debugMomentCheckBox = new CheckBox("Moment", 0, 0, true);
	debugImpulseCheckBox = new CheckBox("Impulse", 0, 0, true);
	debugAngularVelocityCheckBox = new CheckBox("Angular velocity", 0, 0, true);
	debugAngularImpulseCheckBox = new CheckBox("Angular impulse", 0, 0, true);
	debugPointLabel = new Label("Points", 0, 0);
	debugInfoPointCheckBox = new CheckBox("Info", 0, 0, true);
	debugCenterOfMassCheckBox = new CheckBox("Center of mass", 0, 0, true);
	debugIntersectionCheckBox = new CheckBox("Intersections", 0, 0, true);
	debugRenderLabel = new Label("Render", 0, 0);
	debugRenderPiesCheckBox = new CheckBox("Statistics", 0, 0, true);
	debugRenderSpheresCheckBox = new CheckBox("Collision spheres", 0, 0, true);
	debugInfoVectorCheckBox->action = [] (CheckBox* c) { toggleDebugVecType(Debug::INFO_VEC); };
	debugVelocityCheckBox->action = [] (CheckBox* c) { toggleDebugVecType(Debug::VELOCITY); };
	debugAccelerationCheckBox->action = [] (CheckBox* c) { toggleDebugVecType(Debug::ACCELERATION); };
	debugForceCheckBox->action = [] (CheckBox* c) { toggleDebugVecType(Debug::FORCE); };
	debugAngularImpulseCheckBox->action = [] (CheckBox* c) { toggleDebugVecType(Debug::ANGULAR_IMPULSE); };
	debugPositionCheckBox->action = [] (CheckBox* c) { toggleDebugVecType(Debug::POSITION); };
	debugMomentCheckBox->action = [] (CheckBox* c) { toggleDebugVecType(Debug::MOMENT); };
	debugImpulseCheckBox->action = [] (CheckBox* c) { toggleDebugVecType(Debug::IMPULSE); };
	debugAngularVelocityCheckBox->action = [] (CheckBox* c) { toggleDebugVecType(Debug::ANGULAR_VELOCITY); };
	debugInfoPointCheckBox->action = [] (CheckBox* c) { toggleDebugPointType(Debug::INFO_POINT); };
	debugCenterOfMassCheckBox->action = [] (CheckBox* c) { toggleDebugPointType(Debug::CENTER_OF_MASS); };
	debugIntersectionCheckBox->action = [] (CheckBox* c) { toggleDebugPointType(Debug::INTERSECTION); };
	debugRenderPiesCheckBox->action = [] (CheckBox* c) { renderPies = !renderPies; };
	debugRenderSpheresCheckBox->action = [] (CheckBox* c) { renderColissionSpheres = !renderColissionSpheres; };

	debugFrame->add(debugVectorLabel, Align::CENTER);
	debugFrame->add(debugInfoVectorCheckBox, Align::FILL);
	debugFrame->add(debugPositionCheckBox, Align::FILL);
	debugFrame->add(debugVelocityCheckBox, Align::FILL);
	debugFrame->add(debugAccelerationCheckBox, Align::FILL);
	debugFrame->add(debugForceCheckBox, Align::FILL);
	debugFrame->add(debugMomentCheckBox, Align::FILL);
	debugFrame->add(debugImpulseCheckBox, Align::FILL);
	debugFrame->add(debugAngularVelocityCheckBox, Align::FILL);
	debugFrame->add(debugAngularImpulseCheckBox, Align::FILL);
	debugFrame->add(debugPointLabel, Align::CENTER);
	debugFrame->add(debugInfoPointCheckBox, Align::FILL);
	debugFrame->add(debugCenterOfMassCheckBox, Align::FILL);
	debugFrame->add(debugIntersectionCheckBox, Align::FILL);
	debugFrame->add(debugRenderLabel, Align::CENTER);
	debugFrame->add(debugRenderPiesCheckBox, Align::FILL);
	debugFrame->add(debugRenderSpheresCheckBox, Align::FILL);
	
	// Add frames to GUI
	GUI::add(propertiesFrame);
	GUI::add(colorPickerFrame);
	GUI::add(debugFrame);


	// Mouse init
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	mouseVertical = new Panel(0, 0, 0.04, 0.007);
	mouseHorizontal = new Panel(0, 0, 0.007, 0.04);
	mouseVertical->background = Vec4(1);
	mouseHorizontal->background = Vec4(1);
	

	// Picker init
	Picker::init();


	// Origin init
	float originVertices[3] = { 0, 0, 5 };
	originMesh = new ArrayMesh(originVertices, 1, 3, RenderMode::POINTS);
	

	// Vector init
	vectorMesh = new VectorMesh(new float[128 * 9], 128);
	
	
	// Point init
	float* buf = new float[5 * 10];
	for (int i = 0; i < 5; i++) {
		buf[i * 10 + 0] = 0.5f + i;
		buf[i * 10 + 1] = 1.0f; // position
		buf[i * 10 + 2] = 0.5f;

		buf[i * 10 + 3] = 0.02f * i; // size

		buf[i * 10 + 4] = 1.0f;
		buf[i * 10 + 5] = 1.0f; // color 1 orange
		buf[i * 10 + 6] = 0.0f;

		buf[i * 10 + 7] = 0.0f;
		buf[i * 10 + 8] = 0.0f; // color 2 white
		buf[i * 10 + 9] = 0.0f;

	}

	pointMesh = new PointMesh(buf, 5);


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
		screen.dimension = Vec2i(width, height);
		screen.screenFrameBuffer->resize(screen.dimension);
		screen.modelFrameBuffer->resize(screen.dimension);
		screen.blurFrameBuffer->resize(screen.dimension);

		screen.camera.update(((float)width) / ((float)height));
	});

	// Temp
	handler->framebufferResize(width, height);

	// Test
	texture = load("../res/textures/test/disp.jpg");
	BoundingBox box = BoundingBox{ -1,-1,-1,1,1,1 };
	VisualShape shape = VisualShape(box.toShape());
	Vec3f* buffer = new Vec3f[shape.vertexCount];
	shape.computeNormals(buffer);
	shape.normals = SharedArrayPtr<const Vec3f>(buffer);
	mesh = new IndexedMesh(shape);
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
		if (handler->getKey(GLFW_KEY_B)) { debugFrame->visible = true; debugFrame->position = Vec2(0.8); GUI::select(debugFrame); }
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
	orthoMatrix = ortho(-camera.aspect, camera.aspect, -1.0f, 1.0f, -1000.0f, 1000.0f);
	

	// Update picker
	Picker::update(*this, handler->cursorPosition);


	// Update gui
	// Update mouse
	mouseVertical->position = GUI::map(handler->cursorPosition) + Vec2(-mouseVertical->dimension.x / 2, mouseVertical->dimension.y / 2);
	mouseHorizontal->position = GUI::map(handler->cursorPosition) + Vec2(-mouseHorizontal->dimension.x / 2, mouseHorizontal->dimension.y / 2);

	// Update GUI intersection
	GUI::intersect(GUI::map(handler->cursorPosition));
	
	// Update debug frame
	debugInfoVectorCheckBox->checked = debug_enabled[Debug::INFO_VEC];
	debugPositionCheckBox->checked = debug_enabled[Debug::POSITION];
	debugVelocityCheckBox->checked = debug_enabled[Debug::VELOCITY];
	debugMomentCheckBox->checked = debug_enabled[Debug::MOMENT];
	debugForceCheckBox->checked = debug_enabled[Debug::FORCE];
	debugAccelerationCheckBox->checked = debug_enabled[Debug::ACCELERATION];
	debugAngularImpulseCheckBox->checked = debug_enabled[Debug::ANGULAR_IMPULSE];
	debugImpulseCheckBox->checked = debug_enabled[Debug::IMPULSE];
	debugAngularVelocityCheckBox->checked = debug_enabled[Debug::ANGULAR_VELOCITY];
	debugInfoPointCheckBox->checked = point_debug_enabled[Debug::INFO_POINT];
	debugCenterOfMassCheckBox->checked = point_debug_enabled[Debug::CENTER_OF_MASS];
	debugIntersectionCheckBox->checked = point_debug_enabled[Debug::INTERSECTION];
	debugRenderPiesCheckBox->checked = renderPies;
	debugRenderSpheresCheckBox->checked = renderColissionSpheres;

	// Update properties frame
	if (selectedPart) {
		partMeshIDLabel->text = "MeshID: " + std::to_string(selectedPart->drawMeshId);
		renderModeCheckBox->checked = selectedPart->renderMode == GL_LINE;
		partPositionLabel->text = "Position: " + str(selectedPart->cframe.position);
		partNameLabel->text = "Name: " + selectedPart->name;
		partVelocity->text = "Velocity: " + str(selectedPart->parent->velocity);
		partAngularVelocity->text = "Angular Velocity: " + str(selectedPart->parent->angularVelocity);
		double kineticEnergy = selectedPart->parent->getKineticEnergy();
		double potentialEnergy = world->getPotentialEnergyOfPhysical(*(selectedPart->parent));
		partKineticEnergy->text = "Kinetic Energy: " + std::to_string(kineticEnergy);
		partPotentialEnergy->text = "Potential Energy: " + std::to_string(potentialEnergy);
		partEnergy->text = "Energy: " + std::to_string(kineticEnergy + potentialEnergy);

		Vec4 color = selectedPart->material.ambient;
		colorButton->idleColor = color;
		colorButton->hoverColor = color;
		colorButton->pressColor = color;
		colorPicker->setRgba(color);
	} else {
		colorButton->idleColor = Vec4(1);
		colorButton->hoverColor = Vec4(1);
		colorButton->pressColor = Vec4(1);
		partMeshIDLabel->text = "MeshID: -";
		renderModeCheckBox->checked = false;
		partPositionLabel->text = "Position: -";
		partNameLabel->text = "Name: -";
		partVelocity->text = "Velocity: -";
		partAngularVelocity->text = "Angular Velocity: -";
		partKineticEnergy->text = "Kinetic Energy: -";
		partPotentialEnergy->text = "Potential Energy: -";
		partEnergy->text = "Energy: -";
	}
}

void Screen::renderSkybox() {
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	skyboxShader.update(sunDirection);
	skyboxShader.update(camera.viewMatrix, camera.projectionMatrix);
	skyboxTexture->bind();
	sphere->render();
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
}

void Screen::renderPhysicals() {
	std::map<double, ExtendedPart*> transparentMeshes;

	// Bind basic uniforms
	basicShader.updateLight(lights, lightCount);

	SharedLockGuard lg(world->lock);
	
	// Render world objects
	for (ExtendedPart& part : *world) {
		int meshId = part.drawMeshId;

		Material material = part.material;

		// Picker code
		if(&part == selectedPart)
			material.ambient = part.material.ambient + Vec4f(0.1, 0.1, 0.1, -0.2);
		else if (&part == intersectedPart)
			material.ambient = part.material.ambient + Vec4f(-0.1, -0.1, -0.1, 0);
		else
			material.ambient = part.material.ambient;
		
		if (material.ambient.w < 1) {
			transparentMeshes[(camera.cframe.position - part.cframe.position).lengthSquared()] = &part;
			continue;
		}

		basicShader.updateMaterial(material);

		// Render each physical
		basicShader.updatePart(part);

		if(meshId == -1) continue;

		meshes[meshId]->render(part.renderMode);
	}

	for (auto iterator = transparentMeshes.rbegin(); iterator != transparentMeshes.rend(); ++iterator) {
		ExtendedPart* part = (*iterator).second;
		
		Material material = part->material;

		if (part == selectedPart)
			material.ambient = part->material.ambient + Vec4f(0.1, 0.1, 0.1, -0.2);
		else if (part == intersectedPart)
			material.ambient = part->material.ambient + Vec4f(-0.1, -0.1, -0.1, 0);
		else
			material.ambient = part->material.ambient;
		
		basicShader.updateMaterial(material);
	   
		// Render each physical
		basicShader.updatePart(*part);

		if (part->drawMeshId == -1) continue;

		meshes[part->drawMeshId]->render(part->renderMode);
	}
}

void renderSphere(double radius, Vec3 position, Vec4f color) {
	basicShader.updateMaterial(Material(color));

	basicShader.updateModelMatrix(CFrameToMat4(CFrame(position, DiagonalMat3(1,1,1)*radius)));

	sphere->render();
}

void Screen::refresh() {
	fieldIndex = 0;

	//Mat4f f = CFrameToMat4(camera.cframe);
	//camera.cframe = Mat4ToCFrame(lookAt(f, camera.cframe.position, Vec3f()));

	// Shadow setup
	/*Mat4f lightProjection = ortho(-camera.aspect, camera.aspect, -1.0f, 1.0f, 0.1f, 1000.0f);
	Mat4f ligthView = lookAt(camera.cframe.position, camera.cframe.rotation * Vec3f(0, 0, 1), Vec3f(0.0f, 1.0f, 0.0f));
	Mat4f lightMatrix = lightProjection * ligthView;
	depthShader.updateLight(lightMatrix);
	glEnable(GL_DEPTH_TEST);
	depthBuffer->bind();
	//glClear(GL_DEPTH_BUFFER_BIT);
	for (ExtendedPart& part : *world) {
		int meshId = part.drawMeshId;
		depthShader.updateModel(CFrameToMat4(CFramef(part.cframe)));
		meshes[meshId]->render();
	}
	glViewport(0, 0, dimension.x, dimension.y);*/


	// Render skybox
	graphicsMeasure.mark(GraphicsProcess::SKYBOX);
	modelFrameBuffer->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderSkybox();

	glEnable(GL_DEPTH_TEST);


	// Initialize vector log buffer
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	AddableBuffer<AppDebug::ColoredVec>& vecLog = AppDebug::getVecBuffer();
	AddableBuffer<AppDebug::ColoredPoint>& pointLog = AppDebug::getPointBuffer();


	// Render physicals
	graphicsMeasure.mark(GraphicsProcess::PHYSICALS);
	basicShader.update(camera.viewMatrix, camera.projectionMatrix, camera.cframe.position);
	renderPhysicals();

	glDisable(GL_CULL_FACE);
	testShader.updateModel(Mat4f().translate(0, 4, 0));
	testShader.updateView(camera.viewMatrix);
	testShader.update(camera.cframe.position);
	testShader.updateProjection(camera.projectionMatrix);
	testShader.update(texture);
	mesh->renderMode = RenderMode::PATCHES;
	mesh->render(GL_LINE);
	mesh->renderMode = RenderMode::TRIANGLES;
	glEnable(GL_CULL_FACE);

	for (const Physical& p : world->iterPhysicals()) {
		pointLog.add(AppDebug::ColoredPoint(p.getCenterOfMass(), Debug::CENTER_OF_MASS));
	}

	if(selectedPart != nullptr) {
		CFramef selectedCFrame(selectedPart->cframe);
		for(const Vec3f& corner : selectedPart->hitbox.iterVertices()) {
			vecLog.add(AppDebug::ColoredVec(Vec3(selectedCFrame.localToGlobal(corner)), selectedPart->parent->getVelocityOfPoint(Vec3(selectedCFrame.localToRelative(corner))), Debug::VELOCITY));
		}
	}

	if (renderColissionSpheres) {
		for (Physical& phys : world->iterPhysicals()) {
			for (Part& part : phys) {
				Vec4f green = GUI::COLOR::GREEN;
				green.w = 0.5;
				renderSphere(part.maxRadius * 2, part.cframe.getPosition(), green);
			}
		}
		for (Physical& phys : world->iterPhysicals()) {
			Vec4f blue = GUI::COLOR::BLUE;
			blue.w = 0.5;
			renderSphere(phys.circumscribingSphere.radius * 2, phys.circumscribingSphere.origin, blue);
		}
	}


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
				vecLog.add(AppDebug::ColoredVec(part.cframe.localToGlobal(part.hitbox[i]), part.cframe.localToRelative(part.hitbox.normals.get()[i]), Debug::POSITION));
	}*/

	// Update debug meshes
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	updateVecMesh(vectorMesh, vecLog.data, vecLog.size);
	updatePointMesh(pointMesh, pointLog.data, pointLog.size);


	// Render lights
	graphicsMeasure.mark(GraphicsProcess::LIGHTING);
	for (Light light : lights) {
		Mat4f transformation = Mat4f().translate(light.position).scale(0.1f);
		basicShader.updateMaterial(Material(Vec4f(light.color, 1), Vec3f(), Vec3f(), 10));
		basicShader.updateModelMatrix(transformation);
		skyboxMesh->render();
	}

	glDisable(GL_DEPTH_TEST);

	// Render vector mesh
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	vectorShader.update(camera.viewMatrix, camera.projectionMatrix, camera.cframe.position);
	vectorMesh->render();


	// Render point mesh
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	pointShader.update(camera.viewMatrix, camera.projectionMatrix, camera.cframe.position);
	pointMesh->render();

	glEnable(GL_DEPTH_TEST);


	// Render origin mesh
	graphicsMeasure.mark(GraphicsProcess::ORIGIN);
	originShader.update(camera.viewMatrix, camera.cframe.rotation, camera.projectionMatrix, orthoMatrix, camera.cframe.position);
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


	// Render edit tools
	Picker::render(*this, basicShader);


	// Render GUI
	glDisable(GL_DEPTH_TEST);
	graphicsMeasure.mark(GraphicsProcess::OTHER);
	fontShader.update(orthoMatrix);
	GUI::render(orthoMatrix);

	mouseVertical->render();
	mouseHorizontal->render();

	
	// Pie rendering
	graphicsMeasure.mark(GraphicsProcess::PROFILER);
	size_t objCount = world->getPartCount();
	renderDebugField(dimension, font, "Screen", str(dimension) + ", [" + std::to_string(camera.aspect) + ":1]", "");
	renderDebugField(dimension, font, "Position", str(camera.cframe.position), "");
	renderDebugField(dimension, font, "Objects", objCount, "");
	renderDebugField(dimension, font, "Intersections", getTheoreticalNumberOfIntersections(objCount), "");
	renderDebugField(dimension, font, "AVG Collide GJK Iterations", gjkCollideIterStats.avg(), "");
	renderDebugField(dimension, font, "AVG No Collide GJK Iterations", gjkNoCollideIterStats.avg(), "");
	renderDebugField(dimension, font, "TPS", physicsMeasure.getAvgTPS(), "");
	renderDebugField(dimension, font, "FPS", graphicsMeasure.getAvgTPS(), "");
	renderDebugField(dimension, font, "World Kinetic Energy", world->getTotalKineticEnergy(), "");
	renderDebugField(dimension, font, "World Potential Energy", world->getTotalPotentialEnergy(), "");
	renderDebugField(dimension, font, "World Energy", world->getTotalEnergy(), "");

	if (renderPies) {
		float leftSide = float(dimension.x) / float(dimension.y);
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

		ParallelArray<long long, 17> gjkColIter = GJKCollidesIterationStatistics.history.avg();
		ParallelArray<long long, 17> gjkNoColIter = GJKNoCollidesIterationStatistics.history.avg();
		ParallelArray<long long, 17> epaIter = EPAIterationStatistics.history.avg();

		for (size_t i = 0; i < GJKCollidesIterationStatistics.size(); i++) {
			iterationChart.data[0][i] = WeightValue{ (float)gjkColIter[i], std::to_string(gjkColIter[i]) };
			iterationChart.data[1][i] = WeightValue{ (float)gjkNoColIter[i], std::to_string(gjkNoColIter[i]) };
			iterationChart.data[2][i] = WeightValue{ (float)epaIter[i], std::to_string(epaIter[i]) };
		}

		iterationChart.position = Vec2f(-leftSide + 0.1f, -0.3);
		iterationChart.render();
	}


	// Render stuff
	glfwSwapInterval(0);
	glfwSwapBuffers(this->window);
	glfwPollEvents();
}

void Screen::close() {
	basicShader.close();
	depthShader.close();
	vectorShader.close();
	fontShader.close();
	originShader.close();
	skyboxShader.close();
	quadShader.close();
	postProcessShader.close();
	depthShader.close();

	PropertiesParser::write("../res/.properties", properties);

	terminateGL();
}

bool Screen::shouldClose() {
	return glfwWindowShouldClose(window) != 0;
}

int Screen::addMeshShape(const VisualShape& s) {
	int size = (int) meshes.size();
	meshes.push_back(new IndexedMesh(s));
	return size;
}