#include "core.h"

#include "selectionTool.h"

#include "../engine/event/event.h"
#include "../engine/event/mouseEvent.h"
#include "../graphics/gui/gui.h"
#include "../graphics/gui/guiUtils.h"
#include "../graphics/path/path.h"
#include "../graphics/resource/textureResource.h"
#include "../physics/misc/filters/visibilityFilter.h"
#include "../util/resource/resourceManager.h"

#include "../../view/screen.h"
#include "../../application.h"
#include "../../shader/shaders.h"
#include "../../input/standardInputHandler.h"
#include "../../ecs/components.h"
#include "../../worlds.h"
#include "../selectionContext.h"
#include <cmath>


namespace P3D::Graphics {
class TextureResource;
}

namespace P3D::Application {

void SelectionTool::onRegister() {
	auto path = "../res/textures/icons/" + getName() + ".png";
	ResourceManager::add<TextureResource>(getName(), path);
}

void SelectionTool::onDeregister() {
	// Remove texture
}

void SelectionTool::onRender() {
	using namespace Graphics;

	Renderer::beginScene();

	Graphics::Shaders::guiShader.bind();
	Graphics::Shaders::guiShader.setUniform("projectionMatrix", screen.camera.orthoMatrix);

	Path::batch = GUI::batch;
	if (getToolStatus() == kActive) {
		Vec2 a = GUI::map(Vec2(region.x, region.y));
		Vec2 b = GUI::map(handler->getMousePosition());
		Vec2 position(std::min(a.x, b.x), std::min(a.y, b.y));
		Vec2 dimension(std::abs(a.x - b.x), std::abs(a.y - b.y));
		Path::rect(position, dimension);
		Path::batch->submit();
	}

	Renderer::endScene();
}

void SelectionTool::onEvent(Engine::Event& event) {
	using namespace Engine;

	EventDispatcher dispatcher(event);
	dispatcher.dispatch<MousePressEvent>(BIND_EVENT_METHOD(SelectionTool::onMousePress));
	dispatcher.dispatch<MouseReleaseEvent>(BIND_EVENT_METHOD(SelectionTool::onMouseRelease));
}

bool SelectionTool::onMousePress(Engine::MousePressEvent& event) {
	using namespace Engine;

	if (event.getButton() == Mouse::LEFT) {
		Vec2i position = Vec2i(event.getX(), event.getY());

		region.x = position.x;
		region.y = position.y;

		if (!event.getModifiers().isCtrlPressed()) 
			screen.selectionContext.selection.clear();

		setToolStatus(kActive);
	}

	return false;
}

bool SelectionTool::onMouseRelease(Engine::MouseReleaseEvent& event) {
	using namespace Engine;

	if (event.getButton() == Mouse::LEFT) {
		Vec2i position = Vec2i(event.getX(), event.getY());
		
		region.z = position.x;
		region.w = position.y;

		setToolStatus(kIdle);

		Vec4 mappedRegion = GUI::mapRegion(region, Vec2(0, screen.dimension.x), Vec2(screen.dimension.y, 0), Vec2(-1, 1), Vec2(-1, 1));
		auto[left, right] = GUI::minmax(mappedRegion.x, mappedRegion.z);
		auto[down, up] = GUI::minmax(mappedRegion.y, mappedRegion.w);
		//VisibilityFilter visibilityFilter = VisibilityFilter::forWindow(screen.camera.cframe.position, screen.camera.getForwardDirection(), screen.camera.getUpDirection(), screen.camera.fov, screen.camera.aspect, screen.camera.zfar);
		VisibilityFilter visibilityFilter = VisibilityFilter::forSubWindow(screen.camera.cframe.position, screen.camera.getForwardDirection(), screen.camera.getUpDirection(), screen.camera.fov, screen.camera.aspect, screen.camera.zfar, left, right, down, up);

		auto view = screen.registry.view<Comp::Hitbox, Comp::Transform>();
		for (auto entity : view) {
			Ref<Comp::Hitbox> hitbox = view.get<Comp::Hitbox>(entity);
			Ref<Comp::Transform> transform = view.get<Comp::Transform>(entity);
			screen.world->syncReadOnlyOperation([&] () {
				Shape shape = hitbox->getShape();
				if (!transform->isPartAttached())
					shape = shape.scaled(transform->getScale());
				
				Bounds bounds = shape.getBounds(transform->getRotation()) + transform->getPosition();
				if (!visibilityFilter(bounds))
					return;

				screen.selectionContext.selection.add(entity);
			});
		}
	}

	return false;
};

};
