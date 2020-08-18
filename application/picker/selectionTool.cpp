#include "core.h"

#include "selectionTool.h"

#include "../engine/event/event.h"
#include "../engine/event/mouseEvent.h"
#include "../engine/ecs/entity.h"
#include "../graphics/gui/gui.h"
#include "../graphics/gui/guiUtils.h"
#include "../graphics/path/path.h"
#include "../shader/shaders.h"

#include "../view/screen.h"
#include "../application.h"
#include "../input/standardInputHandler.h"
#include "../application.h"
#include "../view/screen.h"
#include "../worlds.h"
#include "../physics/misc/filters/visibilityFilter.h"

#include <cmath>

namespace P3D::Application {

void SelectionTool::onRender() {
	using namespace Graphics;

	Renderer::beginScene();

	Graphics::Shaders::guiShader.bind();
	Graphics::Shaders::guiShader.setUniform("projectionMatrix", screen.camera.orthoMatrix);

	Path::batch = GUI::batch;
	if (getToolStatus() == kActive) {
		Vec2 a = GUI::map(Vec2(currentRegion.x, currentRegion.y));
		Vec2 b = GUI::map(handler->getMousePosition());
		Vec2 position(std::min(a.x, b.x), std::min(a.y, b.y));
		Vec2 dimension(std::abs(a.x - b.x), std::abs(a.y - b.y));
		Path::rect(position, dimension);
	}

	for (const Region& region : regions) {
		Vec2 a = GUI::map(Vec2(region.x, region.y));
		Vec2 b = GUI::map(Vec2(region.z, region.w));
		Vec2 position(std::min(a.x, b.x), std::min(a.y, b.y));
		Vec2 dimension(std::abs(a.x - b.x), std::abs(a.y - b.y));
		Path::rect(position, dimension);
	}

	Path::batch->submit();

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

		currentRegion.x = position.x;
		currentRegion.y = position.y;

		if (!event.getModifiers().isCtrlPressed()) 
			regions.clear();

		setToolStatus(kActive);
	}

	return false;
}

bool SelectionTool::onMouseRelease(Engine::MouseReleaseEvent& event) {
	using namespace Engine;

	if (event.getButton() == Mouse::LEFT) {
		Vec2i position = Vec2i(event.getX(), event.getY());
		
		currentRegion.z = position.x;
		currentRegion.w = position.y;

		regions.push_back(currentRegion);

		setToolStatus(kIdle);

		screen.world->syncReadOnlyOperation([this] () {
			Camera& camera = screen.camera;

			Vec4 mappedRegion = GUI::mapRegion(currentRegion, Vec2(0, screen.dimension.x), Vec2(0, screen.dimension.y), Vec2(-1, 1), Vec2(-1, 1));
			auto[left, right] = GUI::minmax(mappedRegion.x, mappedRegion.z);
			auto[down, up] = GUI::minmax(mappedRegion.y, mappedRegion.w);

			VisibilityFilter boundingboxFilter = VisibilityFilter::forSubWindow(camera.cframe.position, camera.getForwardDirection(), camera.getUpDirection(), camera.fov, camera.aspect, camera.zfar, left, right, down, up);
			for (ExtendedPart& part : screen.world->iterPartsFiltered(boundingboxFilter, ALL_PARTS)) {
				if (!boundingboxFilter(part.getCenterOfMass()))
					continue;
				//selection.insert(&part);
			}
		});
	}

	return false;
};

};
