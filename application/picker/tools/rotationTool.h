#pragma once

#include "../graphics/glfwUtils.h"
#include "../engine/event/mouseEvent.h"
#include "../engine/tool/stateTool.h"
#include "ecs/components.h"

namespace P3D::Application {

class RotationTool : public Engine::StateTool {
private:
	enum SelectionToolStatus : Engine::ToolStatus {
		kIdle = 0,
		kRotateX = 1,
		kRotateY = 2,
		kRotateZ = 3,
		kRotateC = 4,
		kTranslateC = 5
	};

	bool active = false;

public:
	DEFINE_TOOL("Rotate", "Rotate entities by using the handles", Graphics::GLFW::Cursor::ARROW);

	void onRegister() override;
	void onDeregister() override;
	void onRender() override;
	void onUpdate() override;
	void onEvent(Engine::Event& event) override;

	bool onMousePress(Engine::MousePressEvent& event);
	bool onMouseRelease(Engine::MouseReleaseEvent& event);
	bool onMouseDrag(Engine::MouseDragEvent& event);

	static void rotateAroundLine(const Vec3& direction, bool local = true);
};

};