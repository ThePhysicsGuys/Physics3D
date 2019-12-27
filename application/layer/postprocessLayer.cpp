#include "core.h"

#include "postprocessLayer.h"

#include "../graphics/buffers/frameBuffer.h"
#include "../graphics/mesh/primitive.h"
#include "../graphics/renderUtils.h"
#include "../graphics/texture.h"

#include "../application/shader/shaders.h"
#include "../view/screen.h"

namespace Application {

PostprocessLayer::PostprocessLayer() : Layer() {

}

PostprocessLayer::PostprocessLayer(Screen* screen, char flags) : Layer("Postprocess", screen, flags) {

}

void PostprocessLayer::onInit() {

}

void PostprocessLayer::onUpdate() {

}

void PostprocessLayer::onEvent(::Event& event) {

}

void PostprocessLayer::onRender() {
	screen->screenFrameBuffer->unbind();
	Renderer::clearColor();
	Renderer::clearDepth();

	Renderer::disableDepthTest();
	ApplicationShaders::postProcessShader.updateTexture(screen->screenFrameBuffer->texture);
	screen->quad->render();
}

void PostprocessLayer::onClose() {

}

};