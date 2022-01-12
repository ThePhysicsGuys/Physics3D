#include "core.h"

#include "regionSelectionTool.h"

#include "../engine/event/event.h"
#include "../engine/event/mouseEvent.h"
#include "../graphics/gui/gui.h"
#include "../graphics/gui/guiUtils.h"
#include "../graphics/path/path.h"
#include "../graphics/resource/textureResource.h"
#include <Physics3D/boundstree/filters/visibilityFilter.h>
#include "../util/resource/resourceManager.h"
#include "selectionTool.h"

#include "../../view/screen.h"
#include "../../application.h"
#include "../../shader/shaders.h"
#include "../../input/standardInputHandler.h"
#include "../../ecs/components.h"
#include "../../worlds.h"
#include <cmath>

namespace P3D::Application {

	void RegionSelectionTool::onRegister() {
		auto path = "../res/textures/icons/" + getName() + ".png";
		ResourceManager::add<TextureResource>(getName(), path);
	}

	void RegionSelectionTool::onDeregister() {
		// Remove texture
	}

	void RegionSelectionTool::onRender() {
		using namespace Graphics;

		Renderer::beginScene();

		Graphics::Shaders::guiShader->bind();
		Graphics::Shaders::guiShader->setUniform("projectionMatrix", screen.camera.orthoMatrix);

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

	void RegionSelectionTool::onEvent(Engine::Event& event) {
		using namespace Engine;

		EventDispatcher dispatcher(event);
		dispatcher.dispatch<MousePressEvent>(EVENT_BIND(RegionSelectionTool::onMousePress));
		dispatcher.dispatch<MouseReleaseEvent>(EVENT_BIND(RegionSelectionTool::onMouseRelease));
	}

	bool RegionSelectionTool::onMousePress(Engine::MousePressEvent& event) {
		using namespace Engine;

		if (event.getButton() != Mouse::LEFT)
			return false;

		region.x = event.getX();
		region.y = event.getY();

		// Multi selection check
		if (!event.getModifiers().isCtrlPressed())
			SelectionTool::clear();

		setToolStatus(kActive);

		return false;
	}

	bool RegionSelectionTool::onMouseRelease(Engine::MouseReleaseEvent& event) {
		using namespace Engine;

		if (event.getButton() != Mouse::LEFT)
			return false;

		region.z = event.getX();
		region.w = event.getY();

		if (region.x == region.z && region.y == region.w) { // Single selection
			auto intersectedEntity = SelectionTool::getIntersectedEntity();

			if (intersectedEntity.has_value())
				SelectionTool::toggle(intersectedEntity->first);
		} else { // Region selection
			Vec4 mappedRegion = GUI::mapRegion(region, Vec2(0, screen.dimension.x), Vec2(screen.dimension.y, 0), Vec2(-1, 1), Vec2(-1, 1));
			auto [left, right] = GUI::minmax(mappedRegion.x, mappedRegion.z);
			auto [down, up] = GUI::minmax(mappedRegion.y, mappedRegion.w);
			VisibilityFilter visibilityFilter = VisibilityFilter::forSubWindow(screen.camera.cframe.position, screen.camera.getForwardDirection(), screen.camera.getUpDirection(), screen.camera.fov, screen.camera.aspect, screen.camera.zfar, left, right, down, up);

			auto view = screen.registry.view<Comp::Hitbox, Comp::Transform>();
			std::shared_lock<UpgradeableMutex> worldReadLock(*screen.worldMutex);
			for (auto entity : view) {
				IRef<Comp::Hitbox> hitbox = view.get<Comp::Hitbox>(entity);
				IRef<Comp::Transform> transform = view.get<Comp::Transform>(entity);

				Shape shape = hitbox->getShape();
				if (!transform->isRootPart())
					shape = shape.scaled(transform->getScale());

				Bounds bounds = shape.getBounds(transform->getRotation()) + transform->getPosition();
				if (visibilityFilter(bounds))
					SelectionTool::select(entity);
			}
		}

		setToolStatus(kIdle);


		return false;
	}
}
