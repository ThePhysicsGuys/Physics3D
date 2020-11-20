#pragma once

#include "../graphics/glfwUtils.h"
#include "../engine/event/mouseEvent.h"
#include "../engine/tool/stateTool.h"
#include <set>


namespace Engine {
class Event;
class Entity;
}

struct ExtendedPart;

namespace P3D::Application {

class SelectionTool : public Engine::StateTool {
	typedef Vec4i Region;

private:
	enum SelectionToolStatus : Engine::ToolStatus {
		kIdle = 0,
		kActive = 1
	};

	Region currentRegion;
	std::vector<Region> regions;

	std::set<ExtendedPart*> selection;

public:
	DEFINE_TOOL("Select", "Selects one or multiple entities.", Graphics::GLFW::Cursor::CROSSHAIR);

	void onDeselect() override;
	void onRegister() override;
	void onDeregister() override;
	void onRender() override;
	void onEvent(Engine::Event& event) override;

	bool onMousePress(Engine::MousePressEvent& event);
	bool onMouseRelease(Engine::MouseReleaseEvent& event);
};

};