#include "core.h"

#include "postprocessLayer.h"

#include "../graphics/buffers/frameBuffer.h"

#include "../graphics/mesh/primitive.h"

#include "../graphics/shader/shaders.h"
#include "../graphics/renderUtils.h"
#include "../graphics/texture.h"
#include "view/screen.h"

PostprocessLayer::PostprocessLayer() : Layer() {

}

PostprocessLayer::PostprocessLayer(Screen* screen, char flags) : Layer("Postprocess", screen, flags) {

}

void PostprocessLayer::onInit() {

}

void PostprocessLayer::onUpdate() {

}

void PostprocessLayer::onEvent(Event& event) {

}

void PostprocessLayer::onRender() {
	screen->screenFrameBuffer->unbind();
	Renderer::clearColor();
	Renderer::clearDepth();

	Renderer::disableDepthTest();
	GraphicsShaders::quadShader.updateProjection(Matrix<double, 4, 4>::IDENTITY());
	GraphicsShaders::quadShader.updateTexture(screen->screenFrameBuffer->texture);
	screen->quad->render();
}

void PostprocessLayer::onClose() {

}