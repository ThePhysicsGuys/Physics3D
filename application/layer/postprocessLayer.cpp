#include "core.h"

#include "postprocessLayer.h"

#include "../graphics/buffers/frameBuffer.h"

#include "../graphics/mesh/primitive.h"

#include "../application/shader/shaders.h"
#include "../graphics/renderUtils.h"
#include "../graphics/texture.h"
#include "view/screen.h"

void PostprocessLayer::onInit() {

}

void PostprocessLayer::onUpdate() {

}

void PostprocessLayer::onEvent(Event& event) {

}

void PostprocessLayer::onRender() {
	Screen* screen = static_cast<Screen*>(this->ptr);

	screen->screenFrameBuffer->unbind();
	Renderer::clearColor();
	Renderer::clearDepth();

	Renderer::disableDepthTest();
	ApplicationShaders::postProcessShader.updateTexture(screen->screenFrameBuffer->texture);
	screen->quad->render();
}

void PostprocessLayer::onClose() {

}