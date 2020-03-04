#include "core.h"

#include "screen.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../graphics/renderer.h"
#include "../graphics/renderUtils.h"
#include "../graphics/texture.h"
#include "shader/shaders.h"

#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/mesh/primitive.h"
#include "../graphics/debug/visualDebug.h"
#include "../graphics/buffers/frameBuffer.h"
#include "../engine/options/keyboardOptions.h"
#include "../input/standardInputHandler.h"
#include "../graphics/meshLibrary.h"
#include "../graphics/visualShape.h"
#include "../worlds.h"
#include "../engine/event/windowEvent.h"
#include "../util/resource/resourceManager.h"
#include "layer/skyboxLayer.h"
#include "layer/modelLayer.h"
#include "layer/constraintLayer.h"
#include "layer/testLayer.h"
#include "layer/pickerLayer.h"
#include "layer/postprocessLayer.h"
#include "layer/guiLayer.h"
#include "layer/debugLayer.h"
#include "layer/debugOverlay.h"

#include "../physics/geometry/shapeClass.h"
#include "../../engine/meshRegistry.h"

#include "frames.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace Application {

std::vector<Engine::Entity*> Screen::entities;

bool initGLFW() {
	// Set window hints
	//Renderer::setGLFWMultisampleSamples(4);

	// Initialize GLFW
	if (!Graphics::Renderer::initGLFW()) {
		Log::error("GLFW failed to initialize");
		return false;
	}

	//Renderer::enableMultisampling();

	Log::info("Initialized GLFW");

	return true;
}

bool initGLEW() {
	// Init GLEW after creating a valid rendering context
	if (!Graphics::Renderer::initGLEW()) {
		terminateGLFW();

		Log::error("GLEW Failed to initialize!");

		return false;
	}

	Log::info("Initialized GLEW");
	return true;
}

void terminateGLFW() {
	Log::info("Closing GLFW");
	Graphics::Renderer::terminateGLFW();
	Log::info("Closed GLFW");
}

Screen::Screen() {

};

Screen::Screen(int width, int height, PlayerWorld* world) {
	this->world = world;

	// Create a windowed mode window and its OpenGL context 
	Graphics::Renderer::createGLFWContext(width, height, "Physics3D");

	if (!Graphics::Renderer::validGLFWContext()) {
		Log::fatal("Invalid rendering context");
		terminateGLFW();
		exit(-1);
	}

	// Make the window's context current 
	Graphics::Renderer::makeGLFWContextCurrent();

	Log::info("OpenGL vendor: (%s)", Graphics::Renderer::getVendor());
	Log::info("OpenGL renderer: (%s)", Graphics::Renderer::getRenderer());
	Log::info("OpenGL version: (%s)", Graphics::Renderer::getVersion());
	Log::info("OpenGL shader version: (%s)", Graphics::Renderer::getShaderVersion());
}


// Handler
StandardInputHandler* handler = nullptr;

// Layers
SkyboxLayer skyboxLayer;
ModelLayer modelLayer;
ConstraintLayer constraintLayer;
TestLayer testLayer;
PickerLayer pickerLayer;
PostprocessLayer postprocessLayer;
GuiLayer guiLayer;
DebugLayer debugLayer;
DebugOverlay debugOverlay;


void Screen::onInit() {

	// Log init
	Log::setLogLevel(Log::Level::INFO);

	// Properties init
	properties = PropertiesParser::read("../res/.properties");

	// load options from properties
	KeyboardOptions::load(properties);

	// Library init
	Graphics::Library::onInit();

	// InputHandler init
	handler = new StandardInputHandler(Graphics::Renderer::getGLFWContext(), *this);

	// Screen size init
	dimension = Graphics::Renderer::getGLFWWindowSize();

	// Framebuffer init
	quad = new ::Quad();
	screenFrameBuffer = new ::FrameBuffer(dimension.x, dimension.y);

	// Shader init
	ApplicationShaders::onInit();

	// Layer creation
	skyboxLayer = SkyboxLayer(this);
	modelLayer = ModelLayer(this);
	testLayer = TestLayer(this, Layer::Disabled);
	constraintLayer = ConstraintLayer(this, Layer::NoUpdate | Layer::NoEvents);
	debugLayer = DebugLayer(this);
	pickerLayer = PickerLayer(this);
	postprocessLayer = PostprocessLayer(this);
	guiLayer = GuiLayer(this);
	debugOverlay = DebugOverlay(this);

	layerStack.pushLayer(&skyboxLayer);
	layerStack.pushLayer(&modelLayer);
	layerStack.pushLayer(&constraintLayer);
	layerStack.pushLayer(&debugLayer);
	layerStack.pushLayer(&pickerLayer);
	layerStack.pushLayer(&postprocessLayer);
	layerStack.pushLayer(&guiLayer);
	layerStack.pushLayer(&testLayer);
	layerStack.pushOverlay(&debugOverlay);

	// Layer init
	layerStack.onInit();

	// Eventhandler init
	eventHandler.setWindowResizeCallback([] (Screen& screen, Vec2i dimension) {
		screen.camera.onUpdate(((float) dimension.x) / ((float) dimension.y));
		screen.dimension = dimension;
		screen.screenFrameBuffer->resize(screen.dimension);
		GUI::blurFrameBuffer->resize(screen.dimension);
	});

	// Camera init
	camera.setPosition(Position(1.0, 1.0, -2.0));
	camera.setRotation(Vec3(0, 3.1415, 0.0));
	camera.onUpdate(1.0, camera.aspect, 0.01, 10000.0);

	// Resize
	FrameBufferResizeEvent event(dimension.x, dimension.y);
	handler->onFrameBufferResize(event);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void) io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(Graphics::Renderer::getGLFWContext(), true);
	ImGui_ImplOpenGL3_Init("#version 130");
}


void Screen::onUpdate() {
	std::chrono::time_point<std::chrono::steady_clock> curUpdate = std::chrono::steady_clock::now();
	std::chrono::nanoseconds deltaTnanos = curUpdate - this->lastUpdate;
	this->lastUpdate = curUpdate;

	double speedAdjustment = deltaTnanos.count() * 0.000000001 * 60.0;

	// IO events
	if (handler->anyKey) {
		bool leftDragging = handler->leftDragging;
		if (handler->getKey(KeyboardOptions::Move::forward))
			camera.move(*this, 0, 0, -1 * speedAdjustment, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::backward))
			camera.move(*this, 0, 0, 1 * speedAdjustment, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::right))
			camera.move(*this, 1 * speedAdjustment, 0, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::left))
			camera.move(*this, -1 * speedAdjustment, 0, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::ascend))
			if (camera.flying) camera.move(*this, 0, 1 * speedAdjustment, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::descend))
			if (camera.flying) camera.move(*this, 0, -1 * speedAdjustment, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::left))
			camera.rotate(*this, 0, 1 * speedAdjustment, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::right))
			camera.rotate(*this, 0, -1 * speedAdjustment, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::up))
			camera.rotate(*this, 1 * speedAdjustment, 0, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::down))
			camera.rotate(*this, -1 * speedAdjustment, 0, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Application::close))
			Graphics::Renderer::closeGLFWWindow();
	}

	// Update camera
	camera.onUpdate();

	// Update layers
	layerStack.onUpdate();
}

void Screen::onEvent(::Event& event) {

	// Consume ImGui events
	if (event.inCategory(EventCategoryKeyboard | EventCategoryMouseButton) || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive()) {
		ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureKeyboard || io.WantTextInput | io.WantCaptureMouse) {
			event.handled = true;
			return;
		}
	}

	camera.onEvent(event);
	layerStack.onEvent(event);
}

void Screen::onRender() {

	// Init imgui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Render to screen Framebuffer
	screenFrameBuffer->bind();
	Graphics::Renderer::enableBlending();
	Graphics::Renderer::enableCulling();
	Graphics::Renderer::enableDepthTest();
	Graphics::Renderer::enableDepthMask();
	Graphics::Renderer::standardBlendFunction();
	Graphics::Renderer::clearColor();
	Graphics::Renderer::clearDepth();

	// Render layers
	layerStack.onRender();

	// Render imgui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	graphicsMeasure.mark(GraphicsProcess::FINALIZE);

	// Finalize
	Graphics::Renderer::swapGLFWInterval(0);
	Graphics::Renderer::swapGLFWBuffers();
	Graphics::Renderer::pollGLFWEvents();

	graphicsMeasure.mark(GraphicsProcess::OTHER);
}

void Screen::onClose() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	screenFrameBuffer->close();

	layerStack.onClose();

	Graphics::Library::onClose();

	ResourceManager::close();

	ApplicationShaders::onClose();

	KeyboardOptions::save(properties);

	PropertiesParser::write("../res/.properties", properties);

	terminateGLFW();
}

bool Screen::shouldClose() {
	return Graphics::Renderer::isGLFWWindowClosed();
}

};