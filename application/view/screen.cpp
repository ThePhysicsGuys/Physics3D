#include "core.h"

#include "screen.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../graphics/renderer.h"
#include "../graphics/glfwUtils.h"
#include "../graphics/texture.h"
#include "shader/shaders.h"

#include "../graphics/mesh/primitive.h"
#include "../graphics/debug/visualDebug.h"
#include "../graphics/buffers/frameBuffer.h"
#include "../engine/options/keyboardOptions.h"
#include "../input/standardInputHandler.h"
#include "../graphics/meshLibrary.h"
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
#include "ecs/components.h"
#include "frames.h"
#include "../util/systemVariables.h"

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
	using namespace Graphics;

	// Init registry component order
	registry.init<Comp::Name, Comp::Transform, Comp::Collider>();
	
	this->world = world;

	// Create a windowed mode window and its OpenGL context 
	GLFWwindow* context = Graphics::GLFW::createContext(width, height, "Physics3D");

	if (!GLFW::validContext(context)) {
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

	Log::info("OpenGL vendor: (%s)", Renderer::getVendor());
	Log::info("OpenGL renderer: (%s)", Renderer::getRenderer());
	Log::info("OpenGL version: (%s)", Renderer::getVersion());
	Log::info("OpenGL shader version: (%s)", Renderer::getShaderVersion());

	SystemVariables::set("OPENGL_SHADER_VERSION", Renderer::parseShaderVersion(Renderer::getShaderVersion()));
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


void Screen::onInit(bool quickBoot) {
	// Log init
	Log::setLogLevel(Log::Level::INFO);

	// Properties init
	properties = Util::PropertiesParser::read("../res/.properties");

	// load options from properties
	Engine::KeyboardOptions::load(properties);

	// Library init
	Library::onInit();

	// InputHandler init
	handler = new StandardInputHandler(GLFW::getCurrentContext(), *this);

	// Screen size init
	dimension = GLFW::getWindowSize();

	// Framebuffer init
	quad = new Quad();
	screenFrameBuffer = new FrameBuffer(dimension.x, dimension.y);

	// GShader init
	Shaders::onInit();

	// Layer creation
	imguiLayer = ImGuiLayer(this);
	cameraLayer = CameraLayer(this);
	if(!quickBoot) skyboxLayer = SkyboxLayer(this);
	modelLayer = ModelLayer(this);
	constraintLayer = ConstraintLayer(this, Engine::Layer::NoUpdate | Engine::Layer::NoEvents);
	shadowLayer = ShadowLayer(this);
	debugLayer = DebugLayer(this);
	pickerLayer = PickerLayer(this);
	postprocessLayer = PostprocessLayer(this);
	guiLayer = GuiLayer(this);
	//testLayer = TestLayer(this);
	debugOverlay = DebugOverlay(this);

	if(!quickBoot) layerStack.pushLayer(&skyboxLayer);
	layerStack.pushLayer(&constraintLayer);
	layerStack.pushLayer(&modelLayer);
	layerStack.pushLayer(&shadowLayer);
	layerStack.pushLayer(&debugLayer);
	layerStack.pushLayer(&pickerLayer);
	layerStack.pushLayer(&postprocessLayer);
	layerStack.pushLayer(&guiLayer);
	layerStack.pushLayer(&debugOverlay);
	//layerStack.pushLayer(&testLayer);
	layerStack.pushLayer(&cameraLayer);
	layerStack.pushLayer(&imguiLayer);

	// Layer init
	layerStack.onInit(registry);

	// Resize
	Engine::FrameBufferResizeEvent event(dimension.x, dimension.y);
	handler->onFrameBufferResize(event);

	// Init frames
	Frames::onInit(registry);
}

void Screen::onUpdate() {
	// Update layers
	layerStack.onUpdate(registry);
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

	layerStack.onEvent(registry, event);
}

void Screen::onRender() {
	using namespace Graphics;
	using namespace Renderer;

	// Set default settings
	//defaultSettings(0);

	// Init imgui
	imguiLayer.begin();

	defaultSettings(screenFrameBuffer->getID());

	// Render layers
	layerStack.onRender(registry);

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

	layerStack.onClose(registry);

	Graphics::Library::onClose();

	ResourceManager::close();

	Shaders::onClose();

	Engine::KeyboardOptions::save(properties);

	Util::PropertiesParser::write("../res/.properties", properties);

	terminateGLFW();
}

bool Screen::shouldClose() {
	return Graphics::GLFW::isWindowClosed();
}

};