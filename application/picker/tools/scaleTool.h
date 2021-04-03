#pragma once

#include "../graphics/glfwUtils.h"
#include "../engine/event/mouseEvent.h"
#include "../engine/tool/stateTool.h"
#include "ecs/components.h"

namespace P3D::Application {

class ScaleTool : public Engine::StateTool {
private:
	enum SelectionToolStatus : Engine::ToolStatus {
		kIdle    = 0,
		kScaleX = 1,
		kScaleY = 2,
		kScaleZ = 3,
		kScaleXYZ = 4,
		kScaleXY = 5,
		kScaleXZ = 6,
		kScaleYZ = 7,
		kTranslateC = 8
	};

	bool active = false;

public:
	DEFINE_TOOL("Scale", "Scale entities by using the handles", Graphics::GLFW::Cursor::ARROW);

	~ScaleTool() = default;	

	void onRegister() override;
	void onDeregister() override;
	void onRender() override;
	void onUpdate() override;
	void onEvent(Engine::Event& event) override;

	bool onMousePress(Engine::MousePressEvent& event);
	bool onMouseRelease(Engine::MouseReleaseEvent& event);
	bool onMouseDrag(Engine::MouseDragEvent& event);

	void scaleAlongLine(const Vec3& direction);
	void scaleInPlane(const Vec3& normal);
	void scaleXYZ();
};

};