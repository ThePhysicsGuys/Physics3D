#include "core.h"

#include "guiLayer.h"

#include "view/screen.h"
#include "view/frames.h"
#include "shader/shaders.h"
#include "input/standardInputHandler.h"
#include "../graphics/font.h"
#include "../graphics/renderUtils.h"
#include "../graphics/shader/shaders.h"
#include "../graphics/gui/guiUtils.h"
#include "../engine/input/mouse.h"

#include "../graphics/buffers/frameBuffer.h"

// Font
Font* font = nullptr;

// Frames
PropertiesFrame* propertiesFrame = nullptr;
DebugFrame* debugFrame = nullptr;
EnvironmentFrame* environmentFrame = nullptr;

ImageFrame* originalRender = nullptr;
ImageFrame* blurRender = nullptr;

GuiLayer::GuiLayer() {

};

GuiLayer::GuiLayer(Screen* screen, char flags) : Layer("Gui", screen, flags) {

}


bool onMouseMove(const MouseMoveEvent& event) {
	return GUI::onMouseMove(event.getNewX(), event.getNewY());
}

bool onMousePress(const MousePressEvent& event) {
	if (Mouse::LEFT == event.getButton()) {
		return GUI::onMousePress(event.getX(), event.getY());
	}

	return false;
}

bool onMouseRelease(const MouseReleaseEvent& event) {
	if (Mouse::LEFT == event.getButton()) {
		return GUI::onMouseRelease(event.getX(), event.getY());
	}

	return false;
}

bool onMouseDrag(const MouseDragEvent& event) {
	return GUI::onMouseDrag(event.getOldX(), event.getOldY(), event.getNewX(), event.getNewY());
}

bool onWindowResize(const WindowResizeEvent& event) {
	float aspect = ((float) event.getWidth()) / ((float) event.getHeight());
	Vec2i dimension = Vec2i(event.getWidth(), event.getHeight());
	return GUI::onWindowResize({ dimension, aspect });
}

void GuiLayer::onInit() {
	// GUI init
	GUI::onInit({ screen->dimension, screen->camera.aspect }, screen->screenFrameBuffer);

	// Frames init
	propertiesFrame = new PropertiesFrame(0.75, 0.75);
	environmentFrame = new EnvironmentFrame(0.8, 0.8);
	debugFrame = new DebugFrame(0.7, 0.7);
	originalRender = new ImageFrame(0, 0, "original");
	blurRender = new ImageFrame(0, 0, "blur");
}

void GuiLayer::onUpdate() {
	// Update GUI
	GUI::onUpdate(screen->camera.orthoMatrix);

	// Update GUI intersection
	GUI::intersect(GUI::map(handler->mousePosition));

	// Update frames
	propertiesFrame->update();
	debugFrame->update();
	environmentFrame->update();
	originalRender->update();
	blurRender->update();
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
	// Render GUI
	Renderer::disableDepthTest();
	graphicsMeasure.mark(GraphicsProcess::OTHER);
	ApplicationShaders::fontShader.updateProjection(screen->camera.orthoMatrix);
	GUI::onRender(screen->camera.orthoMatrix);

	originalRender->image->texture = screen->screenFrameBuffer->texture;
	blurRender->image->texture = screen->blurFrameBuffer->texture;
}

void GuiLayer::onClose() {
	GUI::onClose();
}
