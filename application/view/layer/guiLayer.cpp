#include "core.h"

#include "guiLayer.h"

#include "../font.h"
#include "../screen.h"
#include "../gui/frames.h"
#include "../renderUtils.h"
#include "../shaderProgram.h"
#include "../../input/standardInputHandler.h"
#include "../gui/guiUtils.h"

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
	// Font init
	font = new Font("../res/fonts/droid.ttf");

	// GUI init
	GUI::onInit(screen, font);

	// Frames init
	propertiesFrame = new PropertiesFrame(0.75, 0.75);
	environmentFrame = new EnvironmentFrame(0.8, 0.8);
	debugFrame = new DebugFrame(0.7, 0.7);
}

void GuiLayer::onUpdate() {

	// Update GUI intersection
	GUI::intersect(GUI::map(handler->mousePosition));

	// Update frames
	propertiesFrame->update();
	debugFrame->update();
	environmentFrame->update();
}

void GuiLayer::onEvent(Event& event) {
	GUI::onEvent(event);
}

void GuiLayer::onRender() {
	// Render GUI
	Renderer::disableDepthTest();
	graphicsMeasure.mark(GraphicsProcess::OTHER);
	Shaders::fontShader.updateProjection(screen->camera.orthoMatrix);
	GUI::onRender(screen->camera.orthoMatrix);
}

void GuiLayer::onClose() {
	GUI::onClose();
}
