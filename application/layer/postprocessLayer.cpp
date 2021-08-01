#include "core.h"

#include "postprocessLayer.h"

#include "../graphics/buffers/frameBuffer.h"
#include "../graphics/mesh/primitive.h"
#include "../graphics/renderer.h"
#include "../view/screen.h"
#include "engine/event/windowEvent.h"
#include "../shader/shaders.h"
#include "graphics/texture.h"
#include "graphics/resource/textureResource.h"
#include "imgui/imgui.h"
#include "util/resource/resourceManager.h"

namespace P3D::Application {

void PostprocessLayer::onInit(Engine::Registry64& registry) {
	Screen* screen = static_cast<Screen*>(this->ptr);

	horizontalBlurFrameBuffer = std::make_unique<Graphics::HDRFrameBuffer>(screen->dimension.x, screen->dimension.y);
	verticalBlurFrameBuffer = std::make_unique<Graphics::HDRFrameBuffer>(screen->dimension.x, screen->dimension.y);
}

void PostprocessLayer::onUpdate(Engine::Registry64& registry) {
}

void PostprocessLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {
	Engine::EventDispatcher dispatcher(event);
	dispatcher.dispatch<Engine::FrameBufferResizeEvent>(
		[&] (const auto& event) {
			horizontalBlurFrameBuffer->resize(Vec2i(event.getWidth(), event.getHeight()));
			verticalBlurFrameBuffer->resize(Vec2i(event.getWidth(), event.getHeight()));
			return false;
		}
	);
}

void PostprocessLayer::onRender(Engine::Registry64& registry) {
	using namespace Graphics;
	using namespace Renderer;
	Screen* screen = static_cast<Screen*>(this->ptr);
	
	int amount = 10;
	bool firstIteration = true;
	Renderer::disableDepthTest();
	for (int i = 0; i < amount; i++) {
		if (i % 2 == 0) {
			const auto& texture = firstIteration ? screen->screenFrameBuffer->ca1 : horizontalBlurFrameBuffer->texture;
			horizontalBlurFrameBuffer->bind();
			texture->bind();
			Shaders::blurShader->updateUnit(texture->getUnit());
			Shaders::blurShader->updateDirection(true);
		} else {
			const auto& texture = firstIteration ? screen->screenFrameBuffer->ca1 : horizontalBlurFrameBuffer->texture;
			verticalBlurFrameBuffer->bind();
			texture->bind();
			Shaders::blurShader->updateUnit(texture->getUnit());
			Shaders::blurShader->updateDirection(false);
		}

		screen->quad->render();

		if (firstIteration)
			firstIteration = false;
	}

	Renderer::enableDepthTest();
	screen->screenFrameBuffer->bind();

	ImGui::Begin("BlurTest", (bool*) 0);
	ImVec2 min = ImGui::GetWindowContentRegionMin();
	ImVec2 max = ImGui::GetWindowContentRegionMax();
	ImVec2 size = ImVec2(max.x - min.x, max.y - min.y);
	ImGui::Image((void*)verticalBlurFrameBuffer->texture->getID(), size, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
}

void PostprocessLayer::onClose(Engine::Registry64& registry) {

}

};