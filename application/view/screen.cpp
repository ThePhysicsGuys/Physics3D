#include "core.h"

#include "screen.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../graphics/renderer.h"
#include "../graphics/glfwUtils.h"
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
#include "layer/imguiLayer.h"
#include "layer/shadowLayer.h"
#include "layer/cameraLayer.h"

#include "../physics/geometry/shapeClass.h"
#include "../engine/meshRegistry.h"
#include "../engine/ecs/tree.h"

#include "frames.h"

#include "imgui/imgui.h"

struct GLFWwindow;

namespace P3D::Application {

bool initGLFW() {
	// Set window hints
	//Renderer::setGLFWMultisampleSamples(4);

	// Initialize GLFW
	if (!Graphics::GLFW::init()) {
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
	Graphics::GLFW::terminate();
	Log::info("Closed GLFW");
}

Screen::Screen() {

};

Screen::Screen(int width, int height, PlayerWorld* world) {
	this->world = world;

	// Create a windowed mode window and its OpenGL context 
	GLFWwindow* context = Graphics::GLFW::createContext(width, height, "Physics3D");

	if (!Graphics::GLFW::validContext(context)) {
		Log::fatal("Invalid rendering context");
		terminateGLFW();
		exit(-1);
	}

	// Make the window's context current 
	Graphics::GLFW::setCurrentContext(context);
	
	// already set using application resources on windows
#ifndef _WIN32
	Graphics::GLFW::setWindowIconFromPath("../res/textures/logo128.png");
#endif

	Log::info("OpenGL vendor: (%s)", Graphics::Renderer::getVendor());
	Log::info("OpenGL renderer: (%s)", Graphics::Renderer::getRenderer());
	Log::info("OpenGL version: (%s)", Graphics::Renderer::getVersion());
	Log::info("OpenGL shader version: (%s)", Graphics::Renderer::getShaderVersion());
}


// Handler
StandardInputHandler* handler = nullptr;

// Layers
ImGuiLayer imguiLayer;
CameraLayer cameraLayer;
SkyboxLayer skyboxLayer;
ModelLayer modelLayer;
ConstraintLayer constraintLayer;
ShadowLayer shadowLayer;
PickerLayer pickerLayer;
PostprocessLayer postprocessLayer;
GuiLayer guiLayer;
DebugLayer debugLayer;
TestLayer testLayer;
DebugOverlay debugOverlay;


void Screen::onInit() {
	// Log init
	Log::setLogLevel(Log::Level::INFO);

	// Properties init
	properties = Util::PropertiesParser::read("../res/.properties");

	// load options from properties
	KeyboardOptions::load(properties);

	// Library init
	Graphics::Library::onInit();

	// InputHandler init
	handler = new StandardInputHandler(Graphics::GLFW::getCurrentContext(), *this);

	// Screen size init
	dimension = Graphics::GLFW::getWindowSize();

	// Framebuffer init
	quad = new Graphics::Quad();
	screenFrameBuffer = new Graphics::FrameBuffer(dimension.x, dimension.y);

	// GShader init
	Shaders::onInit();

	// Layer creation
	imguiLayer = ImGuiLayer(this);
	cameraLayer = CameraLayer(this);
	skyboxLayer = SkyboxLayer(this);
	modelLayer = ModelLayer(this);
	constraintLayer = ConstraintLayer(this, Layer::NoUpdate | Layer::NoEvents);
	shadowLayer = ShadowLayer(this);
	debugLayer = DebugLayer(this);
	pickerLayer = PickerLayer(this);
	postprocessLayer = PostprocessLayer(this);
	guiLayer = GuiLayer(this);
	testLayer = TestLayer(this);
	debugOverlay = DebugOverlay(this);

	layerStack.pushLayer(&skyboxLayer);
	layerStack.pushLayer(&modelLayer);
	layerStack.pushLayer(&constraintLayer);
	layerStack.pushLayer(&shadowLayer);
	layerStack.pushLayer(&debugLayer);
	layerStack.pushLayer(&pickerLayer);
	layerStack.pushLayer(&postprocessLayer);
	layerStack.pushLayer(&guiLayer);
	layerStack.pushLayer(&debugOverlay);
	layerStack.pushLayer(&testLayer);
	layerStack.pushLayer(&cameraLayer);
	layerStack.pushLayer(&imguiLayer);

	// Layer init
	layerStack.onInit();

	// Resize
	Engine::FrameBufferResizeEvent event(dimension.x, dimension.y);
	handler->onFrameBufferResize(event);

	// Init frames
	BigFrame::onInit();
}

void Screen::onUpdate() {
	Engine::ECSTree* tree = this->world->ecstree;

	// Update layers
	layerStack.onUpdate();
}

void Screen::onEvent(Engine::Event& event) {
	using namespace Engine;

	/*// Consume ImGui events
	if (event.inCategory(EventCategoryKeyboard | EventCategoryMouseButton) || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive()) {
		ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureKeyboard || io.WantTextInput | io.WantCaptureMouse) {
			event.handled = true;
			return;
		}
	}*/

	layerStack.onEvent(event);
}

void Screen::onRender() {
	using namespace Graphics;
	using namespace Graphics::Renderer;

	// Set default settings
	//defaultSettings(0);

	// Init imgui
	imguiLayer.begin();

	defaultSettings(screenFrameBuffer->getID());

	// Render layers
	layerStack.onRender();

	// Render imgui
	imguiLayer.end();

	graphicsMeasure.mark(GraphicsProcess::FINALIZE);

	// Finalize
	GLFW::swapInterval(1);
	GLFW::swapBuffers();
	GLFW::pollEvents();

	graphicsMeasure.mark(GraphicsProcess::OTHER);
}

void Screen::onClose() {
	screenFrameBuffer->close();

	layerStack.onClose();

	Graphics::Library::onClose();

	ResourceManager::close();

	Shaders::onClose();

	KeyboardOptions::save(properties);

	Util::PropertiesParser::write("../res/.properties", properties);

	terminateGLFW();
}

bool Screen::shouldClose() {
	return Graphics::GLFW::isWindowClosed();
}

};