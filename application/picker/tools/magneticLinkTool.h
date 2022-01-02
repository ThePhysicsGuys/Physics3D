#pragma once

#include "engine/tool/buttonTool.h"
#include "graphics/glfwUtils.h"

namespace P3D::Application {

class MagneticLinkTool : public Engine::ButtonTool {
public:
	DEFINE_TOOL("Magnetic Link", "Select two entities to create a magnetic link beteen.", Graphics::GLFW::Cursor::CROSSHAIR);

	~MagneticLinkTool() override = default;

	void onRegister() override;
	void onDeregister() override;
	void onSelect() override;

	static void linkSelection();
};

}
