#pragma once

#include "../batch/guiBatch.h"
#include "../util/resource/resourceManager.h"
#include "../resource/fontResource.h"

namespace P3D::Graphics {
	
class FrameBuffer;
class Font;

namespace GUI {

// Window
struct WindowInfo {
	Vec2i dimension;
	float aspect;
};

// Global
extern WindowInfo windowInfo;
extern Font* font;
	
// Batch
extern GuiBatch* batch;
	
void onInit(const WindowInfo& info, FrameBuffer* screenFrameBuffer);
bool onWindowResize(const WindowInfo& info);
void onUpdate(const Mat4f& orthoMatrix);
void onClose();

}
	
};