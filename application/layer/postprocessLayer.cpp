#include "core.h"

#include "postprocessLayer.h"

#include "../graphics/buffers/frameBuffer.h"
#include "../graphics/mesh/primitive.h"
#include "../graphics/renderer.h"
#include "../graphics/texture.h"

#include "../application/shader/shaders.h"
#include "../view/screen.h"

namespace P3D::Application {

void PostprocessLayer::onInit(Engine::Registry64& registry) {

}

void PostprocessLayer::onUpdate(Engine::Registry64& registry) {

}

void PostprocessLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {

}

void PostprocessLayer::onRender(Engine::Registry64& registry) {
	using namespace Graphics;
	using namespace Graphics::Renderer;
	Screen* screen = static_cast<Screen*>(this->ptr);
}

void PostprocessLayer::onClose(Engine::Registry64& registry) {

}

};