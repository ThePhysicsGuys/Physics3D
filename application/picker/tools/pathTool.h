#pragma once
#include "engine/event/mouseEvent.h"
#include "engine/tool/stateTool.h"
#include "graphics/glfwUtils.h"

namespace P3D::Application {

class PathTool : public Engine::StateTool {
private:
	bool active = false;

	double distance = 20;
	std::vector<Vec3> nodes;

public:
	DEFINE_TOOL("Path tool", "", Graphics::GLFW::Cursor::ARROW);

	~PathTool() override = default;

	void onRegister() override;
	void onDeregister() override;
	void onRender() override;
	void onUpdate() override;
	void onEvent(Engine::Event& event) override;

	bool onMousePress(Engine::MousePressEvent& event);
	bool onMouseRelease(Engine::MouseReleaseEvent& event);
	bool onMouseDrag(Engine::MouseDragEvent& event);
};

};