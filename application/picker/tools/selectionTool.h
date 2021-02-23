#pragma once

#include "../graphics/glfwUtils.h"
#include "../engine/event/mouseEvent.h"
#include "../engine/tool/stateTool.h"
#include "../engine/ecs/registry.h"

struct ExtendedPart;

namespace P3D::Application {

class SelectionTool : public Engine::StateTool {
private:
	enum SelectionToolStatus : Engine::ToolStatus {
		kIdle = 0,
		kActive = 1
	};

	Vec4i region;

public:
	DEFINE_TOOL("Select", "Selects one or multiple entities.", Graphics::GLFW::Cursor::CROSSHAIR);

	void onRegister() override;
	void onDeregister() override;
	void onRender() override;
	void onEvent(Engine::Event& event) override;

	bool onMousePress(Engine::MousePressEvent& event);
	bool onMouseRelease(Engine::MouseReleaseEvent& event);
};

};