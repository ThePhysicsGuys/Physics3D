#include "core.h"

#include "postprocessLayer.h"

#include "../buffers/frameBuffer.h"

#include "../mesh/primitive.h"

#include "../shaderProgram.h"
#include "../renderUtils.h"
#include "../texture.h"
#include "../screen.h"

PostprocessLayer::PostprocessLayer() : Layer() {

}

PostprocessLayer::PostprocessLayer(Screen * screen, char flags) : Layer("Postprocess", screen, flags) {

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
	Shaders::quadShader.updateProjection(Matrix<double, 4, 4>::IDENTITY());
	Shaders::quadShader.updateTexture(screen->screenFrameBuffer->texture);
	screen->quad->render();
}

void PostprocessLayer::onClose() {

}
