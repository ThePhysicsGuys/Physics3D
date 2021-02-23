#pragma once

#include "../graphics/glfwUtils.h"
#include "../engine/event/mouseEvent.h"
#include "../engine/tool/stateTool.h"

namespace P3D::Application {

class TranslationTool : public Engine::StateTool {
private:
	enum SelectionToolStatus : Engine::ToolStatus {
		kIdle = 0,
		kEditX = 1,
		kEditY = 2,
		kEditZ = 4,
		kEditC = 8,
	};

	Vec3 selectedPoint;
	Vec3 intersectedPoint;

public:
	DEFINE_TOOL("Move", "Move entities by clicking and dragging or using the handles", Graphics::GLFW::Cursor::ARROW);

	void onRegister() override;
	void onDeregister() override;
	void onRender() override;
	void onUpdate() override;
	//void onEvent(Engine::Event& event) override;

	//bool onMousePress(Engine::MousePressEvent& event);
	//bool onMouseRelease(Engine::MouseReleaseEvent& event);
};

};