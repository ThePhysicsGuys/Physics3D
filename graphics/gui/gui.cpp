#include "core.h"

#include "gui.h"

namespace P3D::Graphics {

namespace GUI {

// Global
WindowInfo windowInfo;
FrameBuffer* guiFrameBuffer = nullptr;
Font* font = nullptr;

// Batch
GuiBatch* batch;

void onInit(const WindowInfo& info, FrameBuffer* screenFrameBuffer) {
	// Init
	Shaders::onInit();

	GUI::windowInfo = info;
	GUI::batch = new GuiBatch();

	// font
	GUI::font = ResourceManager::add<FontResource>("font", "../res/fonts/droid.ttf");
}

bool onWindowResize(const WindowInfo& info) {
	GUI::windowInfo = info;

	return false;
}

void onUpdate(const Mat4f& orthoMatrix) {
	Shaders::quadShader->updateProjection(orthoMatrix);
}

void onClose() {
	// Shaders
	Shaders::onClose();

	// Fonts
	font->close();
}
	
}

}