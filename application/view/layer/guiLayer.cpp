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

void GuiLayer::init() {
	// Font init
	font = new Font("../res/fonts/droid.ttf");

	// GUI init
	GUI::init(screen, font);

	// Frames init
	propertiesFrame = new PropertiesFrame(0.75, 0.75);
	environmentFrame = new EnvironmentFrame(0.8, 0.8);
	debugFrame = new DebugFrame(0.7, 0.7);
}

void GuiLayer::update() {

	// Update GUI intersection
	GUI::intersect(GUI::map(handler->cursorPosition));

	// Update frames
	propertiesFrame->update();
	debugFrame->update();
	environmentFrame->update();
}

void GuiLayer::render() {
	// Render GUI
	Renderer::disableDepthTest();
	graphicsMeasure.mark(GraphicsProcess::OTHER);
	Shaders::fontShader.updateProjection(screen->camera.orthoMatrix);
	GUI::render(screen->camera.orthoMatrix);
}

void GuiLayer::close() {
	
}
