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

// Font
Font* font = nullptr;

// Frames
PropertiesFrame* propertiesFrame = nullptr;
DebugFrame* debugFrame = nullptr;
EnvironmentFrame* environmentFrame = nullptr;

GuiLayer::GuiLayer() {

};

GuiLayer::GuiLayer(Screen* screen, char flags) : Layer("Gui", screen, flags) {

}

void GuiLayer::onInit() {
	// GUI init
	GUI::onInit(GUI::WindowInfo{ screen->dimension, screen->camera.aspect });

	// Frames init
	propertiesFrame = new PropertiesFrame(0.75, 0.75);
	environmentFrame = new EnvironmentFrame(0.8, 0.8);
	debugFrame = new DebugFrame(0.7, 0.7);
}

void GuiLayer::onUpdate() {
	// Update GUI
	GUI::onUpdate(GUI::WindowInfo{ screen->dimension, screen->camera.aspect }, screen->camera.orthoMatrix);

	// Update GUI intersection
	GUI::intersect(GUI::map(handler->mousePosition));

	// Update frames
	propertiesFrame->update();
	debugFrame->update();
	environmentFrame->update();
}

void GuiLayer::onEvent(Event& event) {
	GUI::onEvent(event); // TODO remove 
}

void GuiLayer::onRender() {
	// Render GUI
	Renderer::disableDepthTest();
	graphicsMeasure.mark(GraphicsProcess::OTHER);
	ApplicationShaders::fontShader.updateProjection(screen->camera.orthoMatrix);
	GUI::onRender(screen->camera.orthoMatrix);
}

void GuiLayer::onClose() {
	GUI::onClose();
}
