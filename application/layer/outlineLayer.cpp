#include "core.h"
#include "outlineLayer.h"

#include "ecs/components.h"
#include "graphics/meshRegistry.h"
#include "picker/tools/selectionTool.h"
#include "shader/shaders.h"
#include "application.h"
#include "engine/event/windowEvent.h"
#include "view/screen.h"
#include "../graphics/mesh/primitive.h"

namespace P3D::Application {
	
void OutlineLayer::onInit(Engine::Registry64& registry) {
	outlineFrameBuffer = std::make_unique<FrameBuffer>(screen.dimension.x, screen.dimension.y);
}

void OutlineLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {
	Engine::EventDispatcher dispatch(event);
	dispatch.dispatch<Engine::FrameBufferResizeEvent>(
		[&] (Engine::FrameBufferResizeEvent& event) {
			outlineFrameBuffer->resize(Vec2i(event.getWidth(), event.getHeight()));
			return false;
		}
	);
}

void OutlineLayer::onRender(Engine::Registry64& registry) {
	// Hitbox drawing
	auto scf = SelectionTool::selection.getCFrame();
	auto shb = SelectionTool::selection.getHitbox();
	if (scf.has_value() && shb.has_value()) {
		VisualData data = MeshRegistry::getOrCreateMeshFor(shb->baseShape);
		Shaders::debugShader->updateModel(scf.value().asMat4WithPreScale(shb->scale));
		MeshRegistry::meshes[data.id]->render();
	}
	
	// Outline preparation
	outlineFrameBuffer->bind();
	Renderer::clearColor();
	Renderer::disableDepthTest();

	// Render selection mask
	Shaders::maskShader->updateProjectionMatrix(screen.camera.projectionMatrix);
	Shaders::maskShader->updateViewMatrix(screen.camera.viewMatrix);
	Shaders::maskShader->updateColor(Colors::WHITE);
	for (auto entity : SelectionTool::selection) {
		if (IRef<Comp::Transform> transform = registry.get<Comp::Transform>(entity); transform.valid()) {
			if (IRef<Comp::Hitbox> hitbox = registry.get<Comp::Hitbox>(entity); hitbox.valid()) {
				Shape shape = hitbox->getShape();

				if (!hitbox->isPartAttached())
					shape = shape.scaled(transform->getScale());

				const VisualData data = MeshRegistry::getOrCreateMeshFor(shape.baseShape);

				Shaders::maskShader->updateModel(transform->getCFrame().asMat4WithPreScale(shape.scale));
				MeshRegistry::meshes[data.id]->render();
			}
		}
	}

	// Blend edges over screen framebuffer
	screen.screenFrameBuffer->bind();
	Renderer::enableBlending();
	outlineFrameBuffer->texture->bind();
	Shaders::outlineShader->updateUnit(outlineFrameBuffer->texture->getUnit());
	Shaders::outlineShader->updateColor(Colors::MAROON);
	screen.quad->render();
}

}
