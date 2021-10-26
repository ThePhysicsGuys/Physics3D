#include "core.h"

#include "gui.h"
#include "../util/resource/resourceManager.h"

namespace P3D::Graphics {

namespace GUI {

// Global
WindowInfo windowInfo;
Font* font = nullptr;

// Batch
GuiBatch* batch;

void onInit(const WindowInfo& info) {
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