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

void PostprocessLayer::init() {

}

void PostprocessLayer::update() {

}

void PostprocessLayer::event(Event& event) {

}

void PostprocessLayer::render() {
	screen->screenFrameBuffer->unbind();
	Renderer::clearColor();
	Renderer::clearDepth();

	Renderer::disableDepthTest();
	Shaders::quadShader.updateProjection(Mat4f());
	Shaders::quadShader.updateTexture(screen->screenFrameBuffer->texture);
	screen->quad->render();
}

void PostprocessLayer::close() {

}
