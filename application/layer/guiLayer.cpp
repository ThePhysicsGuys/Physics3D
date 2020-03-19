#include "core.h"

#include "guiLayer.h"

#include "view/screen.h"
#include "view/frames.h"
#include "shader/shaders.h"
#include "input/standardInputHandler.h"
#include "../graphics/font.h"
#include "../graphics/renderer.h"
#include "../graphics/shader/shaders.h"
#include "../graphics/gui/guiUtils.h"
#include "../engine/input/mouse.h"
#include "../graphics/buffers/frameBuffer.h"
#include "../graphics/gui/gui.h"
#include "../graphics/debug/visualDebug.h"

namespace Application {

bool onMouseMove(const MouseMoveEvent& event) {
	using namespace Graphics;
	return GUI::onMouseMove(event.getNewX(), event.getNewY());
}

bool onMousePress(const MousePressEvent& event) {
	using namespace Graphics;
	if (Mouse::LEFT == event.getButton()) {
		return GUI::onMousePress(event.getX(), event.getY());
	}

	return false;
}

bool onMouseRelease(const MouseReleaseEvent& event) {
	using namespace Graphics;

	if (Mouse::LEFT == event.getButton()) {
		return GUI::onMouseRelease(event.getX(), event.getY());
	}

	return false;
}

bool onMouseDrag(const MouseDragEvent& event) {
	using namespace Graphics;
	return GUI::onMouseDrag(event.getOldX(), event.getOldY(), event.getNewX(), event.getNewY());
}

bool onWindowResize(const WindowResizeEvent& event) {
	using namespace Graphics;
	float aspect = ((float) event.getWidth()) / ((float) event.getHeight());
	Vec2i dimension = Vec2i(event.getWidth(), event.getHeight());
	return GUI::onWindowResize({ dimension, aspect });
}

void GuiLayer::onInit() {
	using namespace Graphics;

	Screen* screen = static_cast<Screen*>(this->ptr);

	// GUI init
	GUI::onInit({ screen->dimension, screen->camera.aspect }, screen->screenFrameBuffer);
	GraphicsShaders::guiShader.init(screen->camera.orthoMatrix);
}

void GuiLayer::onUpdate() {
	using namespace Graphics;

	Screen* screen = static_cast<Screen*>(this->ptr);

	// Update GUI
	GUI::onUpdate(screen->camera.orthoMatrix);

	// Update GUI intersection
	GUI::intersect(GUI::map(handler->mousePosition));
}

void GuiLayer::onEvent(Event& event) {
	EventDispatcher dispatcher(event);
	dispatcher.dispatch<MouseMoveEvent>(onMouseMove);
	dispatcher.dispatch<MousePressEvent>(onMousePress);
	dispatcher.dispatch<MouseReleaseEvent>(onMouseRelease);
	dispatcher.dispatch<MouseDragEvent>(onMouseDrag);
	dispatcher.dispatch<WindowResizeEvent>(onWindowResize);
}

void GuiLayer::onRender() {
	using namespace Graphics;
	using namespace Graphics::Renderer;
	Screen* screen = static_cast<Screen*>(this->ptr);

	beginScene();

	BigFrame::render();

	endScene();
}

void GuiLayer::onClose() {
	using namespace Graphics;
	GUI::onClose();
}

};