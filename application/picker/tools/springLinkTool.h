#pragma once

#include "engine/tool/buttonTool.h"
#include "graphics/glfwUtils.h"

namespace P3D::Application {

class SpringLinkTool : public Engine::ButtonTool {
public:
	DEFINE_TOOL("Spring Link", "Select two entities to create a spring link beteen.", Graphics::GLFW::Cursor::CROSSHAIR);

	~SpringLinkTool() override = default;

	void onRegister() override;
	void onDeregister() override;
	void onSelect() override;

	static void linkSelection();
};

}
