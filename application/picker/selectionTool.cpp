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

namespace Application {

void SelectionTool::onRender() {
	using namespace Graphics;

	Renderer::beginScene();

	GraphicsShaders::guiShader.bind();
	GraphicsShaders::guiShader.setUniform("projectionMatrix", screen.camera.orthoMatrix);	

	Path::batch = GUI::batch;
	if (getToolStatus() == active) {
		Vec2 a = GUI::map(Vec2(currentRegion.x, currentRegion.y));
		Vec2 b = GUI::map(handler->getMousePosition());
		Vec2 position = Vec2f(fmin(a.x, b.x), fmin(a.y, b.y));
		Vec2 dimension = Vec2f(fabs(a.x - b.x), fabs(a.y - b.y));
		Path::rect(position, dimension);
	}

	for (const Region& region : regions) {
		Vec2 a = GUI::map(Vec2(region.x, region.y));
		Vec2 b = GUI::map(Vec2(region.z, region.w));
		Vec2 position = Vec2f(fmin(a.x, b.x), fmin(a.y, b.y));
		Vec2 dimension = Vec2f(fabs(a.x - b.x), fabs(a.y - b.y));
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

		Graphics::GLFW::setCursor(getCursorType());

		setToolStatus(active);
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

		Graphics::GLFW::setCursor(Graphics::GLFW::Cursor::ARROW);

		setToolStatus(idle);
	}

	return false;
};

};
