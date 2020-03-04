#include "core.h"

#include "postprocessLayer.h"

#include "../graphics/buffers/frameBuffer.h"
#include "../graphics/mesh/primitive.h"
#include "../graphics/renderer.h"
#include "../graphics/texture.h"

#include "../application/shader/shaders.h"
#include "../view/screen.h"

namespace Application {

void PostprocessLayer::onInit() {

}

void PostprocessLayer::onUpdate() {

}

void PostprocessLayer::onEvent(::Event& event) {

}

void PostprocessLayer::onRender() {
	Screen* screen = static_cast<Screen*>(this->ptr);

	Graphics::Renderer::beginScene();

	screen->screenFrameBuffer->unbind();
	Graphics::Renderer::clearColor();
	Graphics::Renderer::clearDepth();

	Graphics::Renderer::disableDepthTest();
	ApplicationShaders::postProcessShader.updateTexture(screen->screenFrameBuffer->texture);
	screen->quad->render();

	Graphics::Renderer::endScene();
}

void PostprocessLayer::onClose() {

}

};