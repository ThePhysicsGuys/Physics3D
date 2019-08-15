#include "screen.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <map>

#include "renderUtils.h"
#include "texture.h"
#include "shaderProgram.h"

#include "mesh/indexedMesh.h"
#include "mesh/primitive.h"
#include "debug/visualDebug.h"
#include "buffers/frameBuffer.h"
#include "../options/keyboardOptions.h"
#include "../input/standardInputHandler.h"
#include "../meshLibrary.h"
#include "../visualShape.h"
#include "../util/log.h"
#include "../worlds.h"

#include "layer/layerStack.h"
#include "layer/skyboxLayer.h"
#include "layer/modelLayer.h"
#include "layer/pickerLayer.h"
#include "layer/postprocessLayer.h"
#include "layer/guiLayer.h"
#include "layer/debugLayer.h"
#include "layer/debugOverlay.h"

#include "gui\frames.h"

bool initGLFW() {
	// Set window hints
	//Renderer::setGLFWMultisampleSamples(4);

	// Initialize GLFW
	if (!Renderer::initGLFW()) {
		Log::error("GLFW failed to initialize");
		return false;
	}

	//Renderer::enableMultisampling();

	Log::info("Initialized GLFW");

	return true;
}

bool initGLEW() {
	// Init GLEW after creating a valid rendering context
	if (!Renderer::initGLEW()) {
		terminateGLFW();

		Log::error("GLEW Failed to initialize!");

		return false;
	}

	Log::info("Initialized GLEW");
	return true;
}

void terminateGLFW() {
	Log::info("Closing GLFW");
	Renderer::terminateGLFW();
	Log::info("Closed GLFW");
}

Screen::Screen() {

};

Screen::Screen(int width, int height, MagnetWorld* world) {
	this->world = world;

	// Create a windowed mode window and its OpenGL context 
	Renderer::createGLFWContext(width, height, "Physics3D");

	if (!Renderer::validGLFWContext()) {
		Log::fatal("Invalid rendering context");
		terminateGLFW();
		exit(-1);
	}

	// Make the window's context current 
	Renderer::makeGLFWContextCurrent();

	Log::info("OpenGL vendor: (%s)", Renderer::getVendor());
	Log::info("OpenGL renderer: (%s)", Renderer::getRenderer());
	Log::info("OpenGL version: (%s)", Renderer::getVersion());
	Log::info("OpenGL shader version: (%s)", Renderer::getShaderVersion());
}


// Handler
StandardInputHandler* handler = nullptr;


// Layers
LayerStack layerStack;
SkyboxLayer skyboxLayer;
ModelLayer modelLayer;
PickerLayer pickerLayer;
PostprocessLayer postprocessLayer;
GuiLayer guiLayer;
DebugLayer debugLayer;
DebugOverlay debugOverlay;


void Screen::init() {

	// Log init
	Log::setLogLevel(Log::Level::INFO);


	// Properties init
	properties = PropertiesParser::read("../res/.properties");


	// load options from properties
	KeyboardOptions::load(properties);


	// Library init
	Library::init();


	// Render mode init
	Renderer::enableCulling();
	Renderer::enableDepthTest();


	// InputHandler init
	handler = new StandardInputHandler(Renderer::getGLFWContext(), *this);


	// Screen size init
	dimension = Renderer::getGLFWWindowSize();


	// Framebuffer init
	quad = new Quad();
	screenFrameBuffer = new FrameBuffer(dimension.x, dimension.y);
	blurFrameBuffer = new FrameBuffer(dimension.x, dimension.y);


	// Shader init
	Shaders::init();


	// Layer creation
	skyboxLayer = SkyboxLayer(this);
	modelLayer = ModelLayer(this, Layer::noRender);
	debugLayer = DebugLayer(this, Layer::noRender);
	pickerLayer = PickerLayer(this, Layer::noRender);
	postprocessLayer = PostprocessLayer(this);
	guiLayer = GuiLayer(this);
	debugOverlay = DebugOverlay(this);

	layerStack.pushLayer(&skyboxLayer);
	layerStack.pushLayer(&modelLayer);
	layerStack.pushLayer(&debugLayer);
	layerStack.pushLayer(&pickerLayer);
	layerStack.pushLayer(&postprocessLayer);
	layerStack.pushLayer(&guiLayer);
	layerStack.pushOverlay(&debugOverlay);


	// Layer init
	layerStack.init();


	// Eventhandler init
	eventHandler.setWindowResizeCallback([](Screen& screen, Vec2i dimension) {
		screen.screenFrameBuffer->resize(dimension);
		screen.blurFrameBuffer->resize(dimension);
		GUI::guiFrameBuffer->resize(dimension);

		screen.camera.update(((float)dimension.x) / ((float)dimension.y));
		screen.dimension = dimension;
	});


	// Camera init
	camera.setPosition(Position(1.0, 1.0, -2.0));
	camera.setRotation(Vec3(0, 3.1415, 0.0));
	camera.update(1.0, camera.aspect, 0.01, 10000.0);


	// Resize
	handler->framebufferResize(dimension);
}

void Screen::update() {

	// IO events
	if (handler->anyKey) {
		bool leftDragging = handler->leftDragging;
		if (handler->getKey(KeyboardOptions::Move::forward))  camera.move(*this, 0, 0, -1, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::backward))  camera.move(*this, 0, 0, 1, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::right))  camera.move(*this, 1, 0, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::left))  camera.move(*this, -1, 0, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::ascend))
			if (camera.flying) camera.move(*this, 0, 1, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::descend))
			if (camera.flying) camera.move(*this, 0, -1, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::left))  camera.rotate(*this, 0, -1, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::right)) camera.rotate(*this, 0, 1, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::up))    camera.rotate(*this, -1, 0, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::down))  camera.rotate(*this, 1, 0, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Application::close)) Renderer::closeGLFWWindow();
		if (handler->getKey(KeyboardOptions::Debug::frame)) { guiLayer.debugFrame->visible = true; guiLayer.debugFrame->position = Vec2(0.8); GUI::select(guiLayer.debugFrame); }
	}


	// Update camera
	camera.update();


	// Update layers
	layerStack.update();
}


void Screen::render() {
	// Render to screen Framebuffer
	screenFrameBuffer->bind();
	Renderer::clearColor();
	Renderer::clearDepth();

	
	// Render layers
	layerStack.render();


	// Finalize
	Renderer::swapGLFWInterval(0);
	Renderer::swapGLFWBuffers();
	Renderer::pollGLFWEvents();
}

void Screen::close() {
	Shaders::close();

	KeyboardOptions::save(properties);

	PropertiesParser::write("../res/.properties", properties);

	terminateGLFW();
}

bool Screen::shouldClose() {
	return Renderer::isGLFWWindowClosed();
}

int Screen::addMeshShape(const VisualShape& s) {
	int size = (int) meshes.size();
	meshes.push_back(new IndexedMesh(s));
	return size;
}