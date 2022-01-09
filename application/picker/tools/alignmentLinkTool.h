#pragma once

#include "engine/tool/buttonTool.h"
#include "graphics/glfwUtils.h"

namespace P3D::Application {

class AlignmentLinkTool : public Engine::ButtonTool {
public:
	DEFINE_TOOL("Alignment Link", "Select two entities to create an alignment link beteen.", Graphics::GLFW::Cursor::CROSSHAIR);

	~AlignmentLinkTool() override = default;

	void onRegister() override;
	void onDeregister() override;
	void onSelect() override;

	static void linkSelection();
};

}
