#pragma once

#include "../graphics/glfwUtils.h"
#include "../engine/event/mouseEvent.h"
#include "../engine/tool/stateTool.h"
#include "../engine/ecs/registry.h"
#include "ecs/components.h"

struct ExtendedPart;

namespace P3D::Application {

class RegionSelectionTool : public Engine::StateTool {
private:
	enum SelectionToolStatus : Engine::ToolStatus {
		kIdle = 0,
		kActive = 1
	};

	Vec4i region;

public:
	DEFINE_TOOL("Select region", "Selects one or multiple entities within a region.", Graphics::GLFW::Cursor::CROSSHAIR);

	~RegionSelectionTool() override = default;

	void onRegister() override;
	void onDeregister() override;
	void onRender() override;
	void onEvent(Engine::Event& event) override;

	bool onMousePress(Engine::MousePressEvent& event);
	bool onMouseRelease(Engine::MouseReleaseEvent& event);
};

}