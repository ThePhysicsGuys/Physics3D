#pragma once

#include "../graphics/glfwUtils.h"
#include "../engine/tool/StateTool.h"

namespace Engine {
class Event;
class Entity;
}

namespace Application {

class SelectionTool : public Engine::StateTool {
private:

	enum SelectionToolStatus : Engine::ToolStatus {
		idle = 0,
		active = 1
	};

	typedef Vec4i Region;
	Region currentRegion;
	std::vector<Region> regions;
	std::vector<Engine::Entity*> selection;

public:
	DEFINE_TOOL("Select", "Selects one or multiple entities.", Graphics::GLFW::Cursor::CROSSHAIR);

	virtual void onRender() override;
	virtual void onEvent(Engine::Event& event) override;

	bool onMousePress(Engine::MousePressEvent& event);
	bool onMouseRelease(Engine::MouseReleaseEvent& event);
};

};